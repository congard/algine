#ifndef ALGINE_BUFFER_H
#define ALGINE_BUFFER_H

#include <algine/types.h>

namespace algine {
class Buffer {
public:
    enum Usage {
        StreamDraw = 0x88E0,
        StaticDraw = 0x88E4,
        DynamicDraw = 0x88E8
    };

    Buffer();
    ~Buffer();

    void bind() const;
    void unbind() const;
    void setData(uint size, const void *data, uint usage);
    void updateData(uint offset, uint size, const void *data);

    uint getId() const;

protected:
    explicit Buffer(uint target);

public:
    uint m_target = 0;
    uint m_id = 0;
};
}

#endif //ALGINE_BUFFER_H
