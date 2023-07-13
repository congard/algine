#ifndef ALGINE_RENDERBUFFER_H
#define ALGINE_RENDERBUFFER_H

#include <algine/core/ContextObject.h>
#include <algine/core/RenderbufferPtr.h>
#include <algine/types.h>

#include <vector>

namespace algine {
AL_CONTEXT_OBJECT(Renderbuffer) {
    AL_CONTEXT_OBJECT_IMPL(Renderbuffer)

public:
    Renderbuffer();
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

public:
    static RenderbufferPtr getByName(const std::string &name);
    static Renderbuffer* byName(const std::string &name);

public:
    static std::vector<RenderbufferPtr> publicObjects;

protected:
    uint m_format {};
    uint m_width {}, m_height {};
};
}

#endif /* ALGINE_RENDERBUFFER_H */