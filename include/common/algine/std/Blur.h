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
    struct Params {
        /**
         * Blur texture params
         */
        TextureCreateInfo textureCreateInfo;

        int kernelRadius {8};
        float kernelSigma {6};

        /**
         * Components to be blurred.
         * @note Components must be compatible with
         * the texture format.
         */
        std::string blurComponents {"rgb"};

        /**
         * Whether use linear sampling or not
         * @see https://www.rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
         */
        bool linearSampling {true};
    };

public:
    explicit Blur(const Params &params, Object *parent = defaultParent());

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

    /**
     * Returns the viewport, i.e. width and height
     * of the resulting texture.
     * @return The viewport as a `glm::ivec2`.
     */
    glm::ivec2 getViewport() const;

private:
    void createShaders(const Params &params);
    void configureShaders(const Params &params);

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
