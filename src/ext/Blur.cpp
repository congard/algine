#include <algine/ext/Blur.h>
#include <algine/ext/constants/BlurShader.h>
#include <algine/core/Engine.h>

using namespace tulz;

namespace algine {
Blur::Blur(const TextureCreateInfo &textureCreateInfo) {
    Framebuffer::create(pingpongFb[0], pingpongFb[1]);
    Texture2D::create(pingpongTex[0], pingpongTex[1]);

    pingpongTex[0]->applyTextureCreateInfo(textureCreateInfo);
    pingpongTex[1]->applyTextureCreateInfo(textureCreateInfo);

    for (uint i = 0; i < 2; ++i) {
        pingpongFb[i]->bind();
        pingpongFb[i]->attachTexture(pingpongTex[i], Framebuffer::ColorAttachmentZero);
        pingpongFb[i]->unbind();
    }
}

Blur::~Blur() {
    Framebuffer::destroy(pingpongFb[0], pingpongFb[1]);
    Texture2D::destroy(pingpongTex[0], pingpongTex[1]);
}

void Blur::configureKernel(const uint radius, const float sigma) {
    uint kernelSize = radius * 2 - 1;
    Array<float> kernel = getKernel(kernelSize, sigma);

    // sending to shader center of kernel and right part
    for (auto & pingpongShader : pingpongShaders) {
        pingpongShader->bind();
        for (uint j = 0; j < radius; ++j) {
            ShaderProgram::setFloat(
                pingpongShader->getLocation(BlurShader::Vars::Kernel) + static_cast<int>((radius - 1 - j)), kernel[j]
            );
        }
    }

    Engine::defaultShaderProgram()->bind();
}

void Blur::makeBlur(const Texture2D *const image) {
    horizontal = true;
    firstIteration = true;
    for (uint i = 0; i < amount * 2; ++i) {
        pingpongShaders[horizontal]->bind();
        pingpongFb[horizontal]->bind();

        if (firstIteration) {
            image->use(0);
            firstIteration = false;
        } else {
            pingpongTex[!horizontal]->use(0);
        }

        quadRenderer->draw();
        horizontal = !horizontal;
    }
}

void Blur::setAmount(const uint _amount) {
    amount = _amount;
}

void Blur::setQuadRenderer(QuadRenderer *const _quadRenderer) {
    quadRenderer = _quadRenderer;
}

void Blur::setPingPongShaders(ShaderProgram *const hor, ShaderProgram *const vert) {
    pingpongShaders[0] = hor;
    pingpongShaders[1] = vert;
}

uint Blur::getAmount() const {
    return amount;
}

QuadRenderer *Blur::getQuadRenderer() const {
    return quadRenderer;
}

ShaderProgram *Blur::getPingPongShaders() const {
    return pingpongShaders[0];
}

Texture2D *Blur::get() const {
    return pingpongTex[!horizontal];
}

Texture2D *const *Blur::getPingPongTextures() const {
    return pingpongTex;
}

Framebuffer *const *Blur::getPingPongFramebuffers() const {
    return pingpongFb;
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
