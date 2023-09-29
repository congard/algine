#ifndef ALGINE_EVENT_H
#define ALGINE_EVENT_H

#include <algine/core/math/Size.h>
#include <algine/core/unified/UnifiedEventHandler.h>

#include <any>

namespace algine {
/**
 * Describes events
 * @see XEventHandler, Scene, Widget
 */
class AL_EXPORT Event {
public:
    enum Id {
        None,
        Press,
        Release,
        Click,
        Move,
        KeyPress,
        KeyRelease,
        KeyRepeat,
        WindowSizeChange,
        WindowFocusChange
    };

    using Data = std::any;

public:
    Event();
    explicit Event(Id id, Data data = {});

    void setId(Id id);
    Id getId() const;

    void setData(const Data &data);
    const Data& getData() const;
    Data& data();

    template<typename T>
    T getData() const {
        return std::any_cast<T>(getData());
    }

    template<typename T>
    T data() {
        return std::any_cast<T>(data());
    }

    const PointerInfo& getPointerInfo() const;
    KeyboardKey getKeyboardKey() const;
    const SizeI& getSize() const;
    const FocusInfo& getFocusInfo() const;

    bool isPointer() const;
    bool isKeyboard() const;
    bool isWindow() const;

private:
    Id m_id;
    Data m_data;
};
}

#endif //ALGINE_EVENT_H
