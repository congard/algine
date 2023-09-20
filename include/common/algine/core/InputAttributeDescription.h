#ifndef ALGINE_INPUTATTRIBUTEDESCRIPTION_H
#define ALGINE_INPUTATTRIBUTEDESCRIPTION_H

#include <algine/core/DataType.h>
#include <algine/platform.h>
#include <algine/types.h>

namespace algine {
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
    uint m_count = 4;
    uint m_stride = 0;
    uint m_offset = 0;
};
}

#endif //ALGINE_INPUTATTRIBUTEDESCRIPTION_H
