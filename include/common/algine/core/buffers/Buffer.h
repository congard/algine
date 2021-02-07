#ifndef ALGINE_BUFFER_H
#define ALGINE_BUFFER_H

#include <algine/types.h>

#include <tulz/Array.h>
#include <tulz/macros.h>

namespace algine {
class Buffer {
public:
    enum Usage {
        StreamDraw = 0x88E0,
        StaticDraw = 0x88E4,
        DynamicDraw = 0x88E8
    };

    enum Type {
        Array = 0x8892,
        Index = 0x8893,
        Uniform = 0x8A11
    };

    enum_class(MapMode,
        Read = 0x0001,
        Write = 0x0002,
        InvalidateRange = 0x0004,
        InvalidateBuffer = 0x0008,
        FlushExplicit = 0x0010,
        Unsinchronized = 0x0020
    );

public:
    Buffer();
    ~Buffer();

    void bind() const;
    void unbind() const;
    void setData(uint size, const void *data, uint usage);
    void updateData(uint offset, uint size, const void *data);
    tulz::Array<byte> getData(uint offset, uint size);

    void* mapData(uint offset, uint size, uint access);
    bool unmapData();

    uint size() const;
    uint getId() const;
    uint getType() const;

protected:
    explicit Buffer(uint target);

public:
    uint m_target = 0;
    uint m_id = 0;
};
}

#endif //ALGINE_BUFFER_H
