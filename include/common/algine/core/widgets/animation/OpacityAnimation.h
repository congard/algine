#ifndef ALGINE_OPACITYANIMATION_H
#define ALGINE_OPACITYANIMATION_H

#include <algine/core/widgets/animation/Animation.h>

namespace algine::Widgets {
/**
 * Animates the opacity of the widget
 */
class OpacityAnimation: public Animation {
public:
    OpacityAnimation();
    OpacityAnimation(float from, float to, const InterpolatorPtr &interpolator);

    void setFrom(float from);
    void setTo(float to);

    float getFrom() const;
    float getTo() const;

protected:
    void onAnimate() override;

private:
    float m_from;
    float m_to;
};
}

#endif //ALGINE_OPACITYANIMATION_H
