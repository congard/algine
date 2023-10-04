#ifndef ALGINE_RELATIVELAYOUT_H
#define ALGINE_RELATIVELAYOUT_H

#include <algine/core/widgets/Layout.h>
#include <tulz/static_initializer.h>

#include <list>

namespace algine {
class AL_EXPORT RelativeLayout: public Layout {
    STATIC_INITIALIZER_DECL

public:
    explicit RelativeLayout(Object *parent = GlobalScene::getInstance());

    static void setAbove(Widget *widget, std::string_view name);
    static void setBelow(Widget *widget, std::string_view name);
    static void setToRightOf(Widget *widget, std::string_view name);
    static void setToLeftOf(Widget *widget, std::string_view name);

    static std::string getAbove(Widget *widget);
    static std::string getBelow(Widget *widget);
    static std::string getToRightOf(Widget *widget);
    static std::string getToLeftOf(Widget *widget);

    static void setAlignLeft(Widget *widget, std::string_view name);
    static void setAlignRight(Widget *widget, std::string_view name);
    static void setAlignTop(Widget *widget, std::string_view name);
    static void setAlignBottom(Widget *widget, std::string_view name);

    static std::string getAlignLeft(Widget *widget);
    static std::string getAlignRight(Widget *widget);
    static std::string getAlignTop(Widget *widget);
    static std::string getAlignBottom(Widget *widget);

protected:
    void onMeasure(int &width, int &height) override;
    void onLayout() override;

private:
    class WidgetInfo;

    WidgetInfo& layout_for(Widget *widget, std::list<WidgetInfo> &widgetsInfo);
};
}

#endif //ALGINE_RELATIVELAYOUT_H
