#include <algine/core/widgets/animation/OpacityAnimation.h>
#include <algine/core/widgets/Widget.h>

namespace algine::Widgets {
OpacityAnimation::OpacityAnimation(): Animation() {
    m_from = 0.0f;
    m_to = 1.0f;
}

OpacityAnimation::OpacityAnimation(float from, float to, const InterpolatorPtr &interpolator)
    : Animation()
{
    m_from = from;
    m_to = to;
    setInterpolator(interpolator);
}

void OpacityAnimation::setFrom(float from) {
    m_from = from;
}

void OpacityAnimation::setTo(float to) {
    m_to = to;
}

float OpacityAnimation::getFrom() const {
    return m_from;
}

float OpacityAnimation::getTo() const {
    return m_to;
}

void OpacityAnimation::onAnimate() {
    m_widget->setOpacity(glm::mix(m_from, m_to, m_value));
}
}
