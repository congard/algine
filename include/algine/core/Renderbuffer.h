#ifndef ALGINE_RENDERBUFFER_H
#define ALGINE_RENDERBUFFER_H

#include <algine/types.h>
#include <algine/core/texture/Texture.h>
#include <algine/templates.h>

namespace algine {
class Renderbuffer {
    friend class Engine;

public:
    Renderbuffer();
    ~Renderbuffer();

    void bind();

    /**
     * Specifies the number of color components and format of the renderbuffer
     * @param format
     */
    void setFormat(uint format);

    void setWidth(uint width);
    void setHeight(uint height);
    void setWidthHeight(uint width, uint height);

    void update();
    void unbind();

    uint getFormat() const;
    uint getWidth() const;
    uint getHeight() const;
    uint getId() const;

    implementVariadicCreate(Renderbuffer)
    implementVariadicDestroy(Renderbuffer)

protected:
    uint id = 0;
    uint
        format = Texture::DepthComponent,
        width = 512,
        height = 512;
};

}

#endif /* ALGINE_RENDERBUFFER_H */