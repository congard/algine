#ifndef ALGINE_WIDGETS_ANIMATION_H
#define ALGINE_WIDGETS_ANIMATION_H

#include <algine/core/widgets/animation/Interpolator.h>

namespace algine {
class Widget;
}

namespace algine::Widgets {
/**
 * Superclass for all widget animations
 * @note onAnimate must be implemented in every derived class
 */
class AL_EXPORT Animation {
public:
    Animation();
    virtual ~Animation() = default;

    void start(float pos = 0.0f);
    void stop();
    void animate();

    bool isActive() const;

    void setDuration(int durationMs);
    int getDuration() const;

    /**
     * The same as <code>static_cast\<float\>(getDuration())</code>
     * @return duration casted to float
     */
    float getDurationF() const;

    long getStartTime() const;

    void setPos(float pos);
    float getPos() const;
    float getValue() const;

    void setWidget(Widget *widget);
    Widget* getWidget() const;

    void setInterpolator(InterpolatorPtr interpolator);
    const InterpolatorPtr& getInterpolator() const;

    template<typename T, typename ...Args>
    inline static std::unique_ptr<T> create(Args&&... args) {
        static_assert(std::is_base_of_v<Animation, T>);
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

protected:
    virtual void onStart();
    virtual void onStop();
    virtual void onFinish();
    virtual void onAnimate() = 0;

protected:
    int m_durationMs;
    long m_startMs;
    float m_pos;
    float m_value;
    Widget *m_widget;
    InterpolatorPtr m_interpolator;
};

using AnimationPtr = std::unique_ptr<Animation>;
}

#endif //ALGINE_WIDGETS_ANIMATION_H
