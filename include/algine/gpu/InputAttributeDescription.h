#ifndef ALGINE_INPUTATTRIBUTEDESCRIPTION_H
#define ALGINE_INPUTATTRIBUTEDESCRIPTION_H

#include <algine/core/platform.h>
#include <algine/core/types.h>
#include <algine/gpu/DataType.h>

namespace algine {
// TODO: rename to InputAttribute
class AL_EXPORT InputAttributeDescription {
public:
    static constexpr uint LocationAbsent = -1;

public:
    uint getLocation() const;
    void setLocation(uint location);

    uint getCount() const;
    void setCount(uint count);

    DataType getDataType() const;
    void setDataType(DataType dataType);

    uint getStride() const;
    void setStride(uint stride);

    uint getOffset() const;
    void setOffset(uint offset);

public:
    DataType m_dataType = DataType::Float;
    uint m_location = LocationAbsent;
    uint m_count = 4; // TODO: 1, 2, 3 or 4
    uint m_stride = 0; // TODO: improve doc
    uint m_offset = 0;
};
}

#endif //ALGINE_INPUTATTRIBUTEDESCRIPTION_H
