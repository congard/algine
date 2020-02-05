#include <algine/InputAttributeDescription.h>

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

uint InputAttributeDescription::getFormat() const {
    return m_format;
}

void InputAttributeDescription::setFormat(const uint format) {
    m_format = format;
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