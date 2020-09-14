#ifndef ALGINE_RENDERBUFFER_H
#define ALGINE_RENDERBUFFER_H

#include <algine/templates.h>
#include <algine/types.h>

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
    void setDimensions(uint width, uint height);

    void update();
    void unbind();

    uint getFormat() const;
    uint getWidth() const;
    uint getHeight() const;
    uint getId() const;

    implementVariadicCreate(Renderbuffer)
    implementVariadicDestroy(Renderbuffer)

protected:
    uint m_id;
    uint m_format;
    uint m_width, m_height;
};
}

#endif /* ALGINE_RENDERBUFFER_H */