#ifndef ALGINE_CONTAINER_H
#define ALGINE_CONTAINER_H

#include <algine/core/widgets/Widget.h>
#include <tulz/static_initializer.h>

#include <list>

namespace algine {
class AL_EXPORT Container: public Widget {
    STATIC_INITIALIZER_DECL

public:
    explicit Container(Object *parent = GlobalScene::getInstance());

    void addWidget(Widget *child);
    bool removeWidget(Widget *child, FindOption option = FindOption::FindRecursively);
    Widget* removeWidget(std::string_view name, FindOption option = FindOption::FindRecursively);

    std::forward_list<Widget*> getWidgets();

    Widget* widgetAt(const PointI &point);
    Widget* notContainerAt(const PointI &point);
    Container* containerAt(const PointI &point);

    void fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) override;

protected:
    void onChildRemoved(Object *child) override;

    void onAnimate() override;
    void onMeasure(int &width, int &height) override;
    void onDraw(Painter &painter) override;
};
}

#endif //ALGINE_CONTAINER_H
