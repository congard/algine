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
    requestLayout();
    invalidate();
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
        requestLayout();
        invalidate();
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
            requestLayout();
            invalidate();
            child->setParent(nullptr);
            m_children.erase(it);
            return true;
        }
    }

    return removeChildOfBy(m_children, name);
}

void Container::removeAllChildren() {
    requestLayout();
    invalidate();

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

RectI Container::geometry(const WidgetPtr &widget) const {
    // step 1: check if the widget is a child of the current Container
    for (auto &child : m_children) {
        if (child == widget) {
            return widget->getGeometry();
        }
    }

    // step 2: check children of child Containers
    for (auto &child : m_children) {
        if (auto container = std::dynamic_pointer_cast<Container>(child); container) {
            auto geometry = container->geometry(widget);

            if (geometry.isValid()) {
                geometry.translate(container->getX(), container->getY());
                geometry.translate(container->getPaddingLeft(), container->getPaddingTop());
                return geometry;
            }
        }
    }

    return {-1, -1, -1, -1};
}

void Container::onMeasure(int &width, int &height) {
    Widget::onMeasure(width, height);

    if (m_horizontalPolicy != SizePolicy::Preferred && m_verticalPolicy != SizePolicy::Preferred)
        return;

    auto keepMax = [](int &in, int p) {
        if (in < p) {
            in = p;
        }
    };

    int maxX = -1;
    int maxY = -1;

    for (auto &child : m_children) {
        child->measure();
        auto boundingRect = child->boundingRect();
        keepMax(maxX, boundingRect.getX() + boundingRect.getWidth());
        keepMax(maxY, boundingRect.getY() + boundingRect.getHeight());
    }

    if (m_horizontalPolicy == SizePolicy::Preferred)
        width = maxX;

    if (m_verticalPolicy == SizePolicy::Preferred)
        height = maxY;

    setMeasuredDimension(width, height);

    for (auto &child : m_children) {
        auto policy_v = child->getVerticalSizePolicy();
        auto policy_h = child->getHorizontalSizePolicy();

        if (child->isVisible() && (policy_v == SizePolicy::MatchParent || policy_h == SizePolicy::MatchParent)) {
            child->forceLayout();
        }
    }
}

void Container::onDraw(Painter &painter) {
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
