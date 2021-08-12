#include <algine/core/widgets/Container.h>

#include <algine/core/painter/Painter.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/TypeRegistry.h>

#include <tulz/StringUtils.h>

#include <pugixml.hpp>

#include <algorithm>

namespace algine {
Container::Container()
    : Widget() {}

void Container::addChild(const WidgetPtr &child) {
    setFlag(Widget::Flag::RedrawRequired);
    child->setParent(this);
    m_children.emplace_back(child);
}

// remove child of child from #children array by #value
template<typename T>
inline bool removeChildOfBy(std::list<WidgetPtr> &children, const T &value) {
    return std::any_of(children.begin(), children.end(), [&](WidgetPtr &child) {
        if (auto group = std::dynamic_pointer_cast<Container>(child); group != nullptr) {
            return group->removeChild(value);
        } else {
            return false;
        }
    });
}

bool Container::removeChild(const WidgetPtr &child) {
    if (auto it = std::find(m_children.begin(), m_children.end(), child); it != m_children.end()) {
        setFlag(Widget::Flag::RedrawRequired);
        child->setParent(nullptr);
        m_children.erase(it);
        return true;
    } else {
        return removeChildOfBy(m_children, child);
    }
}

bool Container::removeChild(const std::string &name) {
    for (auto it = m_children.begin(); it != m_children.end(); ++it) {
        if (auto &child = *it; child->getName() == name) {
            setFlag(Widget::Flag::RedrawRequired);
            child->setParent(nullptr);
            m_children.erase(it);
            return true;
        }
    }

    return removeChildOfBy(m_children, name);
}

void Container::removeAllChildren() {
    setFlag(Widget::Flag::RedrawRequired);

    for (auto &child : m_children) {
        child->setParent(nullptr);
    }

    m_children = {};
}

WidgetPtr Container::findChildImpl(const std::string &name) const {
    for (auto &child : m_children) {
        if (child->getName() == name) {
            return child;
        }
    }

    for (auto &child : m_children) {
        if (auto group = std::dynamic_pointer_cast<Container>(child); group != nullptr) {
            if (auto p = group->findChild(name); p != nullptr) {
                return p;
            }
        }
    }

    return nullptr;
}

std::list<WidgetPtr> Container::findChildren(const std::string &regex) const {
    std::list<WidgetPtr> children;

    for (auto &child : m_children) {
        if (!tulz::StringUtils::findRegex(child->getName(), regex).empty()) {
            children.emplace_back(child);
        }

        if (auto group = std::dynamic_pointer_cast<Container>(child); group != nullptr) {
            children.merge(group->findChildren(regex));
        }
    }

    return children;
}

const WidgetPtr& Container::getChildAt(Index index) const {
    auto it = m_children.begin();

    for (int i = 0; i < index; ++i) {
        ++it;
    }

    return *it;
}

const std::list<WidgetPtr>& Container::getChildren() const {
    return m_children;
}

uint Container::getChildCount() const {
    return m_children.size();
}
    
WidgetPtr Container::widgetAt(const PointI &point) {
    // check if the point belongs to content
    // if not, there is nothing to look for
    if (!(point.getX() >= getPaddingLeft() && point.getX() <= getWidth() - getPaddingRight() &&
          point.getY() >= getPaddingTop() && point.getY() <= getHeight() - getPaddingBottom()))
    {
        return nullptr;
    }

    for (auto it = m_children.end(); it != m_children.begin();) {
        --it;

        auto &child = *it;

        if (!child->isVisible()) {
            continue;
        }

        auto localPoint = child->toLocalPoint(point);

        auto lx = localPoint.getX();
        auto ly = localPoint.getY();

        if (lx >= 0 && lx <= child->getWidth() && ly >= 0 && ly <= child->getHeight()) {
            return child;
        }
    }

    return nullptr;
}

WidgetPtr Container::notContainerAt(const PointI &point) {
    auto child = widgetAt(point);

    if (auto container = std::dynamic_pointer_cast<Container>(child); container != nullptr) {
        return container->notContainerAt(container->toLocalPoint(point));
    } else {
        return child;
    }
}

ContainerPtr Container::containerAt(const PointI &point) {
    auto child = widgetAt(point);
    return std::dynamic_pointer_cast<Container>(child);
}

void Container::measure(int &width, int &height) {
    auto keepMax = [](int &in, int p) {
        if (in < p) {
            in = p;
        }
    };

    if (m_horizontalPolicy == SizePolicy::Preferred && m_verticalPolicy == SizePolicy::Preferred) {
        int maxX = -1;
        int maxY = -1;

        for (auto &child : m_children) {
            child->measure();
            auto boundingRect = child->boundingRect();
            keepMax(maxX, boundingRect.getX() + boundingRect.getWidth());
            keepMax(maxY, boundingRect.getY() + boundingRect.getHeight());
        }

        width = maxX;
        height = maxY;

        return;
    }

    switch (m_horizontalPolicy) {
        case SizePolicy::Preferred: {
            int maxX = -1;

            for (auto &child : m_children) {
                child->measure();
                auto boundingRect = child->boundingRect();
                keepMax(maxX, boundingRect.getX() + boundingRect.getWidth());
            }

            width = maxX;
            break;
        }
        default: break;
    }

    switch (m_verticalPolicy) {
        case SizePolicy::Preferred: {
            int maxY = -1;

            for (auto &child : m_children) {
                child->measure();
                auto boundingRect = child->boundingRect();
                keepMax(maxY, boundingRect.getY() + boundingRect.getHeight());
            }

            height = maxY;
            break;
        }
        default: break;
    }
}

void Container::draw(Painter &painter) {
    DisplayOptions options;
    options.parentWidth = getWidth();
    options.parentHeight = getHeight();
    options.parentPaddingLeft = getPaddingLeft();
    options.parentPaddingTop = getPaddingTop();
    options.parentPaddingRight = getPaddingRight();
    options.parentPaddingBottom = getPaddingBottom();
    options.parentFramebuffer = m_framebuffer.get();
    options.painter = &painter;

    for (auto &child : m_children) {
        child->display(options);
    }
}

void Container::fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) {
    Widget::fromXML(node, io);

    for (auto child : node.children()) {
        WidgetPtr childPtr(TypeRegistry::create<Widget>(child.name()));
        childPtr->fromXML(child, io);
        addChild(childPtr);
    }
}
}
