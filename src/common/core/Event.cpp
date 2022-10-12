#include <algine/core/Event.h>

#include <utility>

namespace algine {
Event::Event(): m_id(Id::None) {}

Event::Event(Id id, Data data)
    : m_id(id), m_data(std::move(data)) {}

void Event::setId(Id id) {
    m_id = id;
}

Event::Id Event::getId() const {
    return m_id;
}

void Event::setData(const Event::Data &data) {
    m_data = data;
}

const Event::Data& Event::getData() const {
    return m_data;
}

Event::Data& Event::data() {
    return m_data;
}

const XEventHandler::PointerInfo& Event::getPointerInfo() const {
    return getData<const XEventHandler::PointerInfo&>();
}

KeyboardKey Event::getKeyboardKey() const {
    return getData<KeyboardKey>();
}

const SizeI& Event::getSize() const {
    return getData<const SizeI&>();
}

const XEventHandler::FocusInfo& Event::getFocusInfo() const {
    return getData<const XEventHandler::FocusInfo&>();
}

bool Event::isPointer() const {
    switch (getId()) {
        case Press:
        case Release:
        case Click:
        case Move:
            return true;
        default:
            return false;
    }
}

bool Event::isKeyboard() const {
    switch (getId()) {
        case KeyPress:
        case KeyRelease:
        case KeyRepeat:
            return true;
        default:
            return false;
    }
}

bool Event::isSurface() const {
    switch (getId()) {
        case SurfaceSizeChange:
        case SurfaceFocusChange:
            return true;
        default:
            return false;
    }
}
}
