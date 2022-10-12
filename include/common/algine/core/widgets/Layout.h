#ifndef ALGINE_LAYOUT_H
#define ALGINE_LAYOUT_H

#include <algine/core/widgets/Container.h>

namespace algine {
class Layout: public Container {
public:
    using WidgetPtrView = PtrView<Widget>;

public:
    static void setMargin(WidgetPtrView widget, int left, int top, int right, int bottom);
    static void setMarginLeft(WidgetPtrView widget, int margin);
    static void setMarginTop(WidgetPtrView widget, int margin);
    static void setMarginRight(WidgetPtrView widget, int margin);
    static void setMarginBottom(WidgetPtrView widget, int margin);

    static int getMarginLeft(WidgetPtrView widget);
    static int getMarginTop(WidgetPtrView widget);
    static int getMarginRight(WidgetPtrView widget);
    static int getMarginBottom(WidgetPtrView widget);

    static void setAlignment(WidgetPtrView widget, uint alignment);
    static uint getAlignment(WidgetPtrView widget);

protected:
    static void setChildXDirectly(WidgetPtrView child, int x);
    static void setChildYDirectly(WidgetPtrView child, int y);
    static void childGeometryChanged(WidgetPtrView child, const RectI &geometry);
};
}

#endif //ALGINE_LAYOUT_H
