#ifndef ALGINE_RELATIVELAYOUT_H
#define ALGINE_RELATIVELAYOUT_H

#include <algine/core/widgets/Layout.h>

#include <list>

namespace algine {
class RelativeLayout: public Layout {
public:
    static void setAbove(WidgetPtrView widget, std::string_view name);
    static void setBelow(WidgetPtrView widget, std::string_view name);
    static void setToRightOf(WidgetPtrView widget, std::string_view name);
    static void setToLeftOf(WidgetPtrView widget, std::string_view name);

    static std::string getAbove(WidgetPtrView widget);
    static std::string getBelow(WidgetPtrView widget);
    static std::string getToRightOf(WidgetPtrView widget);
    static std::string getToLeftOf(WidgetPtrView widget);

    static void setAlignLeft(WidgetPtrView widget, std::string_view name);
    static void setAlignRight(WidgetPtrView widget, std::string_view name);
    static void setAlignTop(WidgetPtrView widget, std::string_view name);
    static void setAlignBottom(WidgetPtrView widget, std::string_view name);

    static std::string getAlignLeft(WidgetPtrView widget);
    static std::string getAlignRight(WidgetPtrView widget);
    static std::string getAlignTop(WidgetPtrView widget);
    static std::string getAlignBottom(WidgetPtrView widget);

protected:
    void onMeasure(int &width, int &height) override;
    void onLayout() override;

private:
    class WidgetInfo;

    WidgetInfo& layout_for(Widget *widget, std::list<WidgetInfo> &widgetsInfo);
};
}

#endif //ALGINE_RELATIVELAYOUT_H
