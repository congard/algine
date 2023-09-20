#ifndef ALGINE_COLORANIMATION_H
#define ALGINE_COLORANIMATION_H

#include <algine/core/widgets/animation/Animation.h>
#include <algine/core/Color.h>

namespace algine::Widgets {
/**
 * Animates the background color of the widget
 */
class AL_EXPORT ColorAnimation: public Animation {
public:
    ColorAnimation();
    ColorAnimation(const Color &from, const Color &to, const InterpolatorPtr &interpolator);

    void setFrom(const Color &from);
    void setTo(const Color &to);

    const Color& getFrom() const;
    const Color& getTo() const;

protected:
    void onAnimate() override;

private:
    Color m_from;
    Color m_to;
};
}

#endif //ALGINE_COLORANIMATION_H
