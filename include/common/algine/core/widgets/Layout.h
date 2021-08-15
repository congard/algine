#ifndef ALGINE_LAYOUT_H
#define ALGINE_LAYOUT_H

#include <algine/core/widgets/Container.h>
#include <algine/core/AutoRawPtr.h>

namespace algine {
class Layout: public Container {
public:
    using WidgetAutoRawPtr = AutoRawPtr<Widget>;

public:
    static void setMargin(WidgetAutoRawPtr widget, int left, int top, int right, int bottom);
    static void setMarginLeft(WidgetAutoRawPtr widget, int margin);
    static void setMarginTop(WidgetAutoRawPtr widget, int margin);
    static void setMarginRight(WidgetAutoRawPtr widget, int margin);
    static void setMarginBottom(WidgetAutoRawPtr widget, int margin);

    static int getMarginLeft(WidgetAutoRawPtr widget);
    static int getMarginTop(WidgetAutoRawPtr widget);
    static int getMarginRight(WidgetAutoRawPtr widget);
    static int getMarginBottom(WidgetAutoRawPtr widget);

    static void setAlignment(WidgetAutoRawPtr widget, uint alignment);
    static uint getAlignment(WidgetAutoRawPtr widget);

protected:
    static void setChildXDirectly(WidgetAutoRawPtr child, int x);
    static void setChildYDirectly(WidgetAutoRawPtr child, int y);
    static void childGeometryChanged(WidgetAutoRawPtr child, const RectI &geometry);
};
}

#endif //ALGINE_LAYOUT_H
