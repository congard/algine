#include <algine/ext/Blur.h>
#include <algine/ext/constants/BlurShader.h>
#include <algine/std/MathTools.h>

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
    float kernel[kernelSize];
    MathTools::getBlurKernel(kernel, kernelSize, sigma);

    // sending to shader center of kernel and right part
    for (auto & pingpongShader : pingpongShaders) {
        pingpongShader->use();
        for (uint j = 0; j < radius; ++j) {
            ShaderProgram::setFloat(
                pingpongShader->getLocation(BlurShader::Vars::Kernel) + (radius - 1 - j), kernel[j]
            );
        }
    }

    ShaderProgram::reset();
}

void Blur::makeBlur(const Texture2D *const image) {
    horizontal = true;
    firstIteration = true;
    for (uint i = 0; i < amount * 2; ++i) {
        pingpongShaders[horizontal]->use();
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
}
