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

    template<typename T = Widget>
    Ptr<T> findChild(const std::string &name) const;

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
    void onDraw(Painter &painter) override;

protected:
    std::list<WidgetPtr> m_children;

private:
    WidgetPtr findChildImpl(const std::string &name) const;
};

template<typename T>
Ptr<T> Container::findChild(const std::string &name) const {
    if constexpr(std::is_same_v<T, Widget>) {
        return findChildImpl(name);
    } else {
        return std::dynamic_pointer_cast<T>(findChildImpl(name));
    }
}
}

#endif //ALGINE_CONTAINER_H
