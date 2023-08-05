#ifndef ALGINE_BLUR_H
#define ALGINE_BLUR_H

#include <algine/core/texture/Texture2D.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/shader/ShaderProgram.h>

#include <algine/std/QuadRenderer.h>

#include <tulz/Array.h>

namespace algine {
class Blur: public Object {
public:
    explicit Blur(const TextureCreateInfo &textureCreateInfo, Object *parent = defaultParent());

    /**
     * TODO
     * @param kernelRadius
     * @param blurComponent
     * @param linearSampling
     */
    void configureShaders(uint kernelRadius, const std::string &blurComponent, bool linearSampling = true);

    /**
     * Must be called <b>after</b> setPingPongShaders(h, v)
     * @param radius blur kernel radius
     * @param sigma blur kernel sigma
     */
    void configureKernel(int radius, float sigma, bool linearSampling = true);

    /**
     * <b>quadRenderer must be bound</b>, call blur->getQuadRenderer()->bind() if you need
     * @param image: Texture2D to blur
     */
    void makeBlur(const Texture2D *image);

    void resizeOutput(uint width, uint height);

    void setAmount(uint amount);
    void setQuadRenderer(QuadRenderer *quadRenderer);

    uint getAmount() const;
    QuadRenderer* getQuadRenderer() const;
    Texture2D* get() const;

    static tulz::Array<float> getKernel(int size, float sigma);

private:
    Texture2D *m_pingpongTex[2];
    Framebuffer *m_pingpongFb[2];
    ShaderProgram *m_pingpongShaders[2];
    QuadRenderer *m_quadRenderer;
    uint m_amount;
    bool m_horizontal, m_firstIteration;
};
}

#endif //ALGINE_BLUR_H
