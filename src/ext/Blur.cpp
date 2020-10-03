#include <algine/ext/Blur.h>
#include <algine/ext/constants/BlurShader.h>

#include <algine/core/PtrMaker.h>
#include <algine/core/Engine.h>

using namespace std;
using namespace tulz;

namespace algine {
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

    for (uint i = 0; i < 2; ++i)
    {
        m_pingpongTex[i]->applyTextureCreateInfo(textureCreateInfo);

        m_pingpongFb[i]->bind();
        m_pingpongFb[i]->attachTexture(m_pingpongTex[i], Framebuffer::ColorAttachmentZero);
        m_pingpongFb[i]->unbind();
    }
}

void Blur::configureKernel(const uint radius, const float sigma) {
    uint kernelSize = radius * 2 - 1;
    auto kernel = getKernel(kernelSize, sigma);

    // sending to shader center of kernel and right part
    for (auto & pingpongShader : m_pingpongShaders) {
        pingpongShader->bind();

        for (uint j = 0; j < radius; ++j) {
            ShaderProgram::setFloat(
                pingpongShader->getLocation(BlurShader::Vars::Kernel) + static_cast<int>((radius - 1 - j)), kernel[j]
            );
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

Array<float> Blur::getKernel(const int size, const float sigma) {
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
