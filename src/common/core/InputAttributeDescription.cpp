#include <algine/core/InputAttributeDescription.h>

namespace algine {
uint InputAttributeDescription::getLocation() const {
    return m_location;
}

void InputAttributeDescription::setLocation(const uint location) {
    m_location = location;
}

uint InputAttributeDescription::getCount() const {
    return m_count;
}

void InputAttributeDescription::setCount(const uint count) {
    m_count = count;
}

DataType InputAttributeDescription::getDataType() const {
    return m_dataType;
}

void InputAttributeDescription::setDataType(DataType dataType) {
    m_dataType = dataType;
}

uint InputAttributeDescription::getStride() const {
    return m_stride;
}

void InputAttributeDescription::setStride(const uint stride) {
    m_stride = stride;
}

uint InputAttributeDescription::getOffset() const {
    return m_offset;
}

void InputAttributeDescription::setOffset(const uint offset) {
    m_offset = offset;
}
}