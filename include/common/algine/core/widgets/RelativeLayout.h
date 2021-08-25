#ifndef ALGINE_RELATIVELAYOUT_H
#define ALGINE_RELATIVELAYOUT_H

#include <algine/core/widgets/Layout.h>

#include <list>

namespace algine {
class RelativeLayout: public Layout {
public:
    static void setAbove(WidgetAutoRawPtr widget, std::string_view name);
    static void setBelow(WidgetAutoRawPtr widget, std::string_view name);
    static void setToRightOf(WidgetAutoRawPtr widget, std::string_view name);
    static void setToLeftOf(WidgetAutoRawPtr widget, std::string_view name);

    static std::string getAbove(WidgetAutoRawPtr widget);
    static std::string getBelow(WidgetAutoRawPtr widget);
    static std::string getToRightOf(WidgetAutoRawPtr widget);
    static std::string getToLeftOf(WidgetAutoRawPtr widget);

    static void setAlignLeft(WidgetAutoRawPtr widget, std::string_view name);
    static void setAlignRight(WidgetAutoRawPtr widget, std::string_view name);
    static void setAlignTop(WidgetAutoRawPtr widget, std::string_view name);
    static void setAlignBottom(WidgetAutoRawPtr widget, std::string_view name);

    static std::string getAlignLeft(WidgetAutoRawPtr widget);
    static std::string getAlignRight(WidgetAutoRawPtr widget);
    static std::string getAlignTop(WidgetAutoRawPtr widget);
    static std::string getAlignBottom(WidgetAutoRawPtr widget);

protected:
    void onMeasure(int &width, int &height) override;
    void onLayout() override;

private:
    class WidgetInfo;

    WidgetInfo& layout_for(Widget *widget, std::list<WidgetInfo> &widgetsInfo);
};
}

#endif //ALGINE_RELATIVELAYOUT_H
