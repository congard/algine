#ifndef ALGINE_DIMEN_H
#define ALGINE_DIMEN_H

#include <algine/core/widgets/Unit.h>

#include <string>

namespace algine {
class Dimen {
public:
    Dimen() = default;
    Dimen(float value, Unit unit);

    void setValue(float value);
    void setUnit(Unit unit);

    float getValue() const;
    Unit getUnit() const;

    float pixels() const;

    std::string toString() const;

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
