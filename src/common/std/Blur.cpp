#include <algine/std/Blur.h>

#include <algine/core/shader/ShaderCreator.h>
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

Blur::Blur(const TextureCreateInfo &textureCreateInfo)
    : m_pingpongShaders(),
      m_quadRenderer(),
      m_amount(2),
      m_horizontal(true),
      m_firstIteration(true)
{
    PtrMaker::create(
        m_pingpongFb[0], m_pingpongFb[1],
        m_pingpongTex[0], m_pingpongTex[1]
    );

    for (uint i = 0; i < 2; ++i) {
        m_pingpongTex[i]->applyTextureCreateInfo(textureCreateInfo);

        m_pingpongFb[i]->bind();
        m_pingpongFb[i]->attachTexture(m_pingpongTex[i], Framebuffer::ColorAttachmentZero);
        m_pingpongFb[i]->unbind();
    }
}

void Blur::configureKernel(int radius, float sigma, bool linearSampling) {
    int kernelSize = radius * 2 - 1;
    auto kernel = getKernel(kernelSize, sigma);

    if (!linearSampling) {
        // sending to shader center of kernel and right part
        for (auto & pingpongShader : m_pingpongShaders) {
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

        for (auto &pingpongShader : m_pingpongShaders) {
            pingpongShader->bind();

            for (int i = 0; i < size; ++i) {
                ShaderProgram::setFloat(pingpongShader->getLocation(Vars::Weights) + i, weights[i]);
                ShaderProgram::setFloat(pingpongShader->getLocation(Vars::Offsets) + i, offsets[i]);
            }
        }
    }

    Engine::defaultShaderProgram()->bind();
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

void Blur::setQuadRenderer(const QuadRendererPtr &quadRenderer) {
    m_quadRenderer = quadRenderer;
}

void Blur::setPingPongShaders(const ShaderProgramPtr &hor, const ShaderProgramPtr &vert) {
    m_pingpongShaders[0] = hor;
    m_pingpongShaders[1] = vert;
}

void Blur::setPingPongShaders(const pair<ShaderProgramPtr, ShaderProgramPtr> &pingPong) {
    m_pingpongShaders[0] = pingPong.first;
    m_pingpongShaders[1] = pingPong.second;
}

uint Blur::getAmount() const {
    return m_amount;
}

QuadRendererPtr& Blur::getQuadRenderer() {
    return m_quadRenderer;
}

ShaderProgramPtr* Blur::getPingPongShaders() {
    return m_pingpongShaders;
}

Texture2DPtr& Blur::get() {
    return m_pingpongTex[!m_horizontal];
}

Texture2DPtr* Blur::getPingPongTextures() {
    return m_pingpongTex;
}

FramebufferPtr* Blur::getPingPongFramebuffers() {
    return m_pingpongFb;
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

pair<ShaderProgramPtr, ShaderProgramPtr> Blur::getPingPongShaders(uint kernelRadius, const string &blurComponent, bool linearSampling) {
    ShaderPtr vertex = QuadRenderer::getVertexShader();
    ShaderPtr fragmentPing;
    ShaderPtr fragmentPong;

    ShaderCreator fragmentCreator(Shader::Fragment);
    fragmentCreator.setPath("@algine/Blur.fs.glsl");
    fragmentCreator.define(Settings::KernelRadius, linearSampling ? (kernelRadius / 2 + 1) : kernelRadius);
    fragmentCreator.define(Settings::OutputType, [&]() {
        switch (blurComponent.size()) {
            case 1: return "float";
            case 2: return "vec2";
            case 3: return "vec3";
            case 4: return "vec4";
            default: throw invalid_argument("Must be: 1 <= blurComponent.size() <= 4");
        }
    }());
    fragmentCreator.define(Settings::TexComponent, blurComponent);

    fragmentCreator.define(Settings::Horizontal);
    fragmentPing = fragmentCreator.create();

    fragmentCreator.removeDefinition(Settings::Horizontal);
    fragmentCreator.define(Settings::Vertical);
    fragmentPong = fragmentCreator.create();

    ShaderProgramPtr ping = PtrMaker::make();
    ping->attachShader(*vertex);
    ping->attachShader(*fragmentPing);
    ping->link();
    ping->loadActiveLocations();

    ShaderProgramPtr pong = PtrMaker::make();
    pong->attachShader(*vertex);
    pong->attachShader(*fragmentPong);
    pong->link();
    pong->loadActiveLocations();

    return {ping, pong};
}
}
