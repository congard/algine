#include <algine/std/Blur.h>

#include <algine/core/shader/ShaderBuilder.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/Engine.h>

using namespace std;
using namespace tulz;

#define constant constexpr auto

namespace algine {
namespace Settings {
constant OutputType = "vecout";
constant TexComponent = "texComponent";

constant Vertical = "ALGINE_VERTICAL";
constant Horizontal = "ALGINE_HORIZONTAL";
constant KernelRadius = "KERNEL_RADIUS";
}

namespace Vars {
constant BaseImage = "image";
constant Offsets = "offsets[0]";
constant Weights = "weights[0]";
}

Blur::Blur(const TextureCreateInfo &textureCreateInfo, Object *parent)
    : Object(parent),
      m_pingpongShaders(),
      m_quadRenderer(),
      m_amount(2),
      m_horizontal(true),
      m_firstIteration(true),
      m_pingpongFb(),
      m_pingpongTex()
{
    for (uint i = 0; i < 2; ++i) {
        m_pingpongFb[i] = new Framebuffer(this);
        m_pingpongTex[i] = new Texture2D(m_pingpongFb[i]);

        m_pingpongTex[i]->applyTextureCreateInfo(textureCreateInfo);

        m_pingpongFb[i]->bind();
        m_pingpongFb[i]->attachTexture(m_pingpongTex[i], Framebuffer::ColorAttachmentZero);
        m_pingpongFb[i]->unbind();
    }
}

void Blur::configureShaders(uint kernelRadius, const std::string &blurComponent, bool linearSampling) {
    auto vertex = QuadRenderer::getVertexShader();

    auto programPing = new ShaderProgram(this);
    auto programPong = new ShaderProgram(this);

    Shader* fragmentPing;
    Shader* fragmentPong;

    ShaderBuilder fragmentBuilder(Shader::Type::Fragment);
    fragmentBuilder.setPath("@algine/Blur.fs.glsl");
    fragmentBuilder.define(Settings::KernelRadius, linearSampling ? (kernelRadius / 2 + 1) : kernelRadius);
    fragmentBuilder.define(Settings::OutputType, [&]() {
        switch (blurComponent.size()) {
            case 1: return "float";
            case 2: return "vec2";
            case 3: return "vec3";
            case 4: return "vec4";
            default: throw invalid_argument("Must be: 1 <= blurComponent.size() <= 4");
        }
    }());
    fragmentBuilder.define(Settings::TexComponent, blurComponent);

    fragmentBuilder.define(Settings::Horizontal);
    fragmentBuilder.setParent(programPing);
    fragmentPing = fragmentBuilder.create();

    fragmentBuilder.removeDefinition(Settings::Horizontal);
    fragmentBuilder.define(Settings::Vertical);
    fragmentBuilder.setParent(programPong);
    fragmentPong = fragmentBuilder.create();

    programPing->attachShader(*vertex);
    programPing->attachShader(*fragmentPing);
    programPing->link();
    programPing->loadActiveLocations();

    programPong->attachShader(*vertex);
    programPong->attachShader(*fragmentPong);
    programPong->link();
    programPong->loadActiveLocations();

    m_pingpongShaders[0] = programPing;
    m_pingpongShaders[1] = programPong;
}

void Blur::configureKernel(int radius, float sigma, bool linearSampling) {
    int kernelSize = radius * 2 - 1;
    auto kernel = getKernel(kernelSize, sigma);

    if (!linearSampling) {
        // sending to shader center of kernel and right part
        for (auto pingpongShader : m_pingpongShaders) {
            pingpongShader->bind();

            for (int j = 0; j < radius; ++j) {
                ShaderProgram::setFloat(pingpongShader->getLocation(Vars::Weights) + (radius - 1 - j), kernel[j]);
                ShaderProgram::setFloat(pingpongShader->getLocation(Vars::Offsets) + j, (float) j);
            }
        }
    } else {
        int size = radius / 2 + 1;

        Array<float> weights(size);
        Array<float> offsets(size);

        // Based on: https://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/

        weights[0] = kernel[radius - 1]; // center element
        offsets[0] = 0.0f;

        for (int i = 1; i < size; ++i) {
            auto offset_d_1 = (float) (i * 2 - 1);
            auto offset_d_2 = offset_d_1 + 1;

            int weight_d_1_index = radius + (i - 1) * 2 + 0;
            int weight_d_2_index = weight_d_1_index + 1;

            if (weight_d_2_index == kernel.size()) {
                weights[i] = kernel[weight_d_1_index];
                offsets[i] = offset_d_1;
                break;
            }

            float weight_d_1 = kernel[weight_d_1_index];
            float weight_d_2 = kernel[weight_d_2_index];

            float weight_l_1_2 = weight_d_1 + weight_d_2;
            float offset_l_1_2 = (offset_d_1 * weight_d_1 + offset_d_2 * weight_d_2) / weight_l_1_2;

            weights[i] = weight_l_1_2;
            offsets[i] = offset_l_1_2;
        }

        for (auto pingpongShader : m_pingpongShaders) {
            pingpongShader->bind();

            for (int i = 0; i < size; ++i) {
                ShaderProgram::setFloat(pingpongShader->getLocation(Vars::Weights) + i, weights[i]);
                ShaderProgram::setFloat(pingpongShader->getLocation(Vars::Offsets) + i, offsets[i]);
            }
        }
    }

    ShaderProgram::getDefault()->bind();
}

void Blur::makeBlur(const Texture2D *image) {
    m_horizontal = true;
    m_firstIteration = true;

    for (uint i = 0; i < m_amount * 2; ++i) {
        m_pingpongShaders[m_horizontal]->bind();
        m_pingpongFb[m_horizontal]->bind();

        if (m_firstIteration) {
            image->use(0);
            m_firstIteration = false;
        } else {
            m_pingpongTex[!m_horizontal]->use(0);
        }

        m_quadRenderer->draw();
        m_horizontal = !m_horizontal;
    }
}

void Blur::resizeOutput(uint width, uint height) {
    for (const auto & texture : m_pingpongTex) {
        texture->bind();
        texture->setDimensions(width, height);
        texture->update();
    }
}

void Blur::setAmount(uint amount) {
    m_amount = amount;
}

void Blur::setQuadRenderer(QuadRenderer *quadRenderer) {
    m_quadRenderer = quadRenderer;
}

uint Blur::getAmount() const {
    return m_amount;
}

QuadRenderer* Blur::getQuadRenderer() const {
    return m_quadRenderer;
}

Texture2D* Blur::get() const {
    return m_pingpongTex[!m_horizontal];
}

Array<float> Blur::getKernel(int size, float sigma) {
    Array<float> kernel(size);

    int mean = size / 2;
    float sum = 0; // For accumulating the kernel values

    for (int x = 0; x < size; x++)  {
        kernel[x] = expf(-0.5f * powf((float)(x - mean) / sigma, 2.0));
        sum += kernel[x]; // Accumulate the kernel value
    }

    // Normalize the kernel
    for (int x = 0; x < size; x++)
        kernel[x] /= sum;

    return kernel;
}
}
