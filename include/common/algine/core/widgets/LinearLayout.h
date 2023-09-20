#ifndef ALGINE_LINEARLAYOUT_H
#define ALGINE_LINEARLAYOUT_H

#include <algine/core/widgets/Layout.h>
#include <tulz/static_initializer.h>

namespace algine {
class AL_EXPORT LinearLayout: public Layout {
    STATIC_INITIALIZER_DECL

public:
    enum class Orientation {
        Vertical,
        Horizontal
    };

public:
    void setOrientation(Orientation orientation);
    Orientation getOrientation() const;

    void fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) override;

protected:
    int matchParentWidth(Widget *child) override;
    int matchParentHeight(Widget *child) override;

    void onMeasure(int &width, int &height) override;
    void onLayout() override;

private:
    void place_vertically(Widget *child, int &y);
    void place_horizontally(Widget *child, int &x);

    void measure_vertical_preferred(int &width, int &height);
    void measure_horizontal_preferred(int &width, int &height);

private:
    Orientation m_orientation {Orientation::Vertical};
};
}

#endif //ALGINE_LINEARLAYOUT_H
