#ifndef ALGINE_DIMEN_H
#define ALGINE_DIMEN_H

#include <algine/core/widgets/Unit.h>
#include <algine/platform.h>

#include <string>
#include <string_view>

namespace algine {
class Window;

class AL_EXPORT Dimen {
public:
    Dimen() = default;

    /**
     * Parses the specified string.
     * @throws std::invalid_argument If `parse` returns `false`.
     * @param str The string to parse.
     */
    explicit Dimen(std::string_view str);

    Dimen(float value, Unit unit);

    void setValue(float value);
    void setUnit(Unit unit);

    float getValue() const;
    Unit getUnit() const;

    /**
     * Converts the current dimen to pixels.
     * @param window The window, in which you want
     * to convert dimen to pixels.
     * @return The pixels value.
     */
    float pixels(Window *window) const;

    /**
     * The same as `pixels`, but returns a value of type `T`.
     * @tparam T The type to cast to.
     * @param window The window, in which you want
     * to convert dimen to pixels.
     * @return The pixels value casted to the type `T`.
     */
    template<typename T>
    T pixelsAs(Window *window) const {
        return static_cast<T>(pixels(window));
    }

    std::string toString() const;

    /**
     * Parses a string value as dimen.
     * <br>Supported suffixes:
     * <ul>
     *   <li>px</li>
     *   <li>dp</li>
     * </ul>
     * Supported format:
     * [number][suffix]
     * @note The suffix is required.
     * @param str The string to parse.
     * @return `true`, in case of success, `false` otherwise.
     */
    bool parse(std::string_view str);

private:
    float m_value {0.0f};
    Unit m_unit {Unit::px};
};

inline Dimen::Dimen(float value, Unit unit)
    : m_value(value), m_unit(unit) {}

inline void Dimen::setValue(float value) { m_value = value; }
inline void Dimen::setUnit(Unit unit) { m_unit = unit; }

inline float Dimen::getValue() const { return m_value; }
inline Unit Dimen::getUnit() const { return m_unit; }
}

#endif //ALGINE_DIMEN_H
