#include <algine/core/widgets/animation/ColorAnimation.h>
#include <algine/core/widgets/Widget.h>

namespace algine::Widgets {
ColorAnimation::ColorAnimation(): Animation() {}

ColorAnimation::ColorAnimation(const Color &from, const Color &to, const InterpolatorPtr &interpolator)
    : Animation()
{
    setFrom(from);
    setTo(to);
    setInterpolator(interpolator);
}

void ColorAnimation::setFrom(const Color &from) {
    m_from = from;
}

void ColorAnimation::setTo(const Color &to) {
    m_to = to;
}

const Color& ColorAnimation::getFrom() const {
    return m_from;
}

const Color& ColorAnimation::getTo() const {
    return m_to;
}

void ColorAnimation::onAnimate() {
    m_widget->setBackground(Paint(Color::fromRgbF(
        glm::mix(m_from.redF(), m_to.redF(), m_value),
        glm::mix(m_from.greenF(), m_to.greenF(), m_value),
        glm::mix(m_from.blueF(), m_to.blueF(), m_value),
        glm::mix(m_from.alphaF(), m_to.alphaF(), m_value)
    )));
}
}
