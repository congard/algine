#ifndef ALGINE_RENDERBUFFER_H
#define ALGINE_RENDERBUFFER_H

#include <algine/core/context/ContextObject.h>
#include <algine/types.h>

#include <vector>

namespace algine {
AL_CONTEXT_OBJECT(Renderbuffer) {
    AL_CONTEXT_OBJECT_IMPL(Renderbuffer)

public:
    explicit Renderbuffer(Object *parent = defaultParent());
    ~Renderbuffer() override;

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

protected:
    uint m_format {};
    uint m_width {}, m_height {};
};
}

#endif /* ALGINE_RENDERBUFFER_H */