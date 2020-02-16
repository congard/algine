#ifndef ALGINE_BLUR_H
#define ALGINE_BLUR_H

#include <algine/texture.h>
#include <algine/framebuffer.h>
#include <algine/QuadRenderer.h>
#include <algine/shader.h>

namespace algine {
class Blur {
public:
    explicit Blur(const TextureCreateInfo &textureCreateInfo);
    ~Blur();

    /**
     * Must be called <b>after</b> setPingPongShaders(h, v)
     * @param radius blur kernel radius
     * @param sigma blur kernel sigma
     */
    void configureKernel(uint radius, float sigma);

    /**
     * <b>quadRenderer must be bound</b>, call blur->getQuadRenderer()->bind() if you need
     * @param image: Texture2D to blur
     */
    void makeBlur(const Texture2D *image);

    void setAmount(uint amount);
    void setQuadRenderer(QuadRenderer *quadRenderer);
    void setPingPongShaders(ShaderProgram *hor, ShaderProgram *vert);

    uint getAmount() const;
    QuadRenderer* getQuadRenderer() const;
    ShaderProgram* getPingPongShaders() const;
    Texture2D* get() const;
    Texture2D *const *getPingPongTextures() const;
    Framebuffer *const *getPingPongFramebuffers() const;

protected:
    Texture2D *pingpongTex[2] {nullptr, nullptr};
    Framebuffer *pingpongFb[2] {nullptr, nullptr};
    ShaderProgram *pingpongShaders[2] {nullptr, nullptr};
    QuadRenderer *quadRenderer = nullptr;
    uint amount = 2;
    bool horizontal = true, firstIteration = true;
};
}

#endif //ALGINE_BLUR_H
