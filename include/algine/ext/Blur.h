#ifndef ALGINE_BLUR_H
#define ALGINE_BLUR_H

#include <algine/core/texture/Texture2D.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/shader/ShaderProgram.h>

#include <algine/std/QuadRendererPtr.h>
#include <algine/std/QuadRenderer.h>

#include <tulz/Array.h>

namespace algine {
class Blur {
public:
    explicit Blur(const TextureCreateInfo &textureCreateInfo);

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

    void resizeOutput(uint width, uint height);

    void setAmount(uint amount);
    void setQuadRenderer(const QuadRendererPtr &quadRenderer);
    void setPingPongShaders(const ShaderProgramPtr &hor, const ShaderProgramPtr &vert);

    uint getAmount() const;
    QuadRendererPtr& getQuadRenderer();
    ShaderProgramPtr* getPingPongShaders();
    Texture2DPtr& get();
    Texture2DPtr* getPingPongTextures();
    FramebufferPtr* getPingPongFramebuffers();

    static tulz::Array<float> getKernel(int size, float sigma);

private:
    Texture2DPtr m_pingpongTex[2];
    FramebufferPtr m_pingpongFb[2];
    ShaderProgramPtr m_pingpongShaders[2];
    QuadRendererPtr m_quadRenderer;
    uint m_amount;
    bool m_horizontal, m_firstIteration;
};
}

#endif //ALGINE_BLUR_H
