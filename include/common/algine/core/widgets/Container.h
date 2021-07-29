#ifndef ALGINE_CONTAINER_H
#define ALGINE_CONTAINER_H

#include <algine/core/widgets/Widget.h>
#include <algine/core/widgets/ContainerPtr.h>

#include <list>

namespace algine {
class Container: public Widget {
public:
    Container();

    void addChild(const WidgetPtr &child);
    bool removeChild(const WidgetPtr &child);
    bool removeChild(const std::string &name);
    void removeAllChildren();

    WidgetPtr findChild(const std::string &name) const;
    std::list<WidgetPtr> findChildren(const std::string &regex) const;

    const WidgetPtr& getChildAt(Index index) const;
    const std::list<WidgetPtr>& getChildren() const;

    uint getChildCount() const;

    WidgetPtr widgetAt(const PointI &point);
    WidgetPtr notContainerAt(const PointI &point);
    ContainerPtr containerAt(const PointI &point);

    void fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) override;

protected:
    void measure(int &width, int &height) override;
    void draw(Painter &painter) override;

protected:
    std::list<WidgetPtr> m_children;
};
}

#endif //ALGINE_CONTAINER_H
