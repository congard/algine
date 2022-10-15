#include <algine/core/widgets/animation/Animation.h>
#include <algine/core/Engine.h>

namespace algine::Widgets {
Animation::Animation()
    : m_durationMs(500),
      m_startMs(-1),
      m_pos(0.0f),
      m_value(0.0f),
      m_widget(nullptr),
      m_interpolator(nullptr) {}

void Animation::start(float pos) {
    setPos(pos);
    onStart();
}

void Animation::stop() {
    m_startMs = -1;
    onStop();
}

void Animation::animate() {
    if (!isActive())
        return;

    long delta = Engine::timeFromStart() - m_startMs;
    m_pos = std::min((float) delta / getDurationF(), 1.0f);
    m_value = m_interpolator->interpolate(m_pos);

    onAnimate();

    if (m_pos == 1.0f) {
        m_startMs = -1;
        onFinish();
    }
}

bool Animation::isActive() const {
    return m_startMs > 0;
}

void Animation::setDuration(int durationMs) {
    m_durationMs = durationMs;
}

int Animation::getDuration() const {
    return m_durationMs;
}

float Animation::getDurationF() const {
    return static_cast<float>(m_durationMs);
}

long Animation::getStartTime() const {
    return m_startMs;
}

void Animation::setPos(float pos) {
    m_pos = pos;
    m_startMs = Engine::timeFromStart() - static_cast<long>(pos * getDurationF());
}

float Animation::getPos() const {
    return m_pos;
}

float Animation::getValue() const {
    return m_value;
}

void Animation::setWidget(Widget *widget) {
    m_widget = widget;
}

Widget* Animation::getWidget() const {
    return m_widget;
}

void Animation::setInterpolator(InterpolatorPtr interpolator) {
    m_interpolator = std::move(interpolator);
}

const InterpolatorPtr& Animation::getInterpolator() const {
    return m_interpolator;
}

void Animation::onStart() {}

void Animation::onStop() {}

void Animation::onFinish() {}
}
