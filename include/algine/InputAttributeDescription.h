#ifndef ALGINE_INPUTATTRIBUTEDESCRIPTION_H
#define ALGINE_INPUTATTRIBUTEDESCRIPTION_H

#include <algine/types.h>

namespace algine {
class InputAttributeDescription {
public:
    static constexpr uint LocationAbsent = -1;

public:
    uint getLocation() const;
    void setLocation(uint location);

    uint getCount() const;
    void setCount(uint count);

    uint getFormat() const;
    void setFormat(uint format);

    uint getStride() const;
    void setStride(uint stride);

    uint getOffset() const;
    void setOffset(uint offset);

public:
    uint m_location = LocationAbsent;
    uint m_count = 4;
    uint m_format = 0x1406; // GL_FLOAT TODO: make type constants (for GL_FLOAT, GL_UNSIGNED_BYTE etc)
    uint m_stride = 0;
    uint m_offset = 0;
};
}

#endif //ALGINE_INPUTATTRIBUTEDESCRIPTION_H
