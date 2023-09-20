#ifndef ALGINE_LAYOUT_H
#define ALGINE_LAYOUT_H

#include <algine/core/widgets/Container.h>

namespace algine {
class AL_EXPORT Layout: public Container {
public:
    static void setMargin(Widget *widget, int left, int top, int right, int bottom);
    static void setMarginLeft(Widget *widget, int margin);
    static void setMarginTop(Widget *widget, int margin);
    static void setMarginRight(Widget *widget, int margin);
    static void setMarginBottom(Widget *widget, int margin);

    static int getMarginLeft(Widget *widget);
    static int getMarginTop(Widget *widget);
    static int getMarginRight(Widget *widget);
    static int getMarginBottom(Widget *widget);

    static void setAlignment(Widget *widget, uint alignment);
    static uint getAlignment(Widget *widget);

protected:
    static void setChildXDirectly(Widget *child, int x);
    static void setChildYDirectly(Widget *child, int y);
    static void childGeometryChanged(Widget *child, const RectI &geometry);
};
}

#endif //ALGINE_LAYOUT_H
