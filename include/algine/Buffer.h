#ifndef ALGINE_BUFFER_H
#define ALGINE_BUFFER_H

#include <algine/types.h>
#include <GL/glew.h>

namespace algine {
class Buffer {
public:
    enum Usage {
        StaticDraw = GL_STATIC_DRAW
    };

    Buffer();
    ~Buffer();

    void bind();
    void unbind();
    void setData(uint size, const void *data, uint usage);

    uint getId() const;

protected:
    explicit Buffer(uint target);

public:
    uint m_target = 0;
    uint m_id = 0;
};
}

#endif //ALGINE_BUFFER_H
