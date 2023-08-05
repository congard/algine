#include <algine/core/widgets/Container.h>
#include <algine/core/painter/Painter.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/TypeRegistry.h>

#include <tulz/StringUtils.h>

#include <pugixml.hpp>

#include <algorithm>

namespace algine {
STATIC_INITIALIZER_IMPL(Container) {
    alRegisterType(Container);
}

Container::Container()
    : Widget() {}

void Container::addWidget(Widget *child) {
    requestLayout();
    invalidate();
    addChild(child);
}

bool Container::removeWidget(Widget *child, FindOption option) {
    return removeChild(child, option);
}

Widget* Container::removeWidget(std::string_view name, FindOption option) {
    if (auto widget = findChild<Widget*>(name); widget != nullptr && removeChild(widget, option))
        return widget;
    return nullptr;
}

std::forward_list<Widget*> Container::getWidgets() {
    return findChildren<Widget*>([](Widget*) { return true; }, Object::FindOption::Direct); // reverse reversed
}

Widget* Container::widgetAt(const PointI &point) {
    // check if the point belongs to content
    // if not, there is nothing to look for
    if (!(point.getX() >= getPaddingLeft() && point.getX() <= getWidth() - getPaddingRight() &&
          point.getY() >= getPaddingTop() && point.getY() <= getHeight() - getPaddingBottom()))
    {
        return nullptr;
    }

    // already reversed list
    auto &children = getChildren();

    // from last to first
    for (auto child : children) {
        auto widget = dynamic_cast<Widget*>(child);

        if (!widget)
            continue;

        if (!widget->isVisible()) {
            continue;
        }

        auto localPoint = widget->mapFromParent(point);

        auto lx = localPoint.getX();
        auto ly = localPoint.getY();

        if (lx >= 0 && lx <= widget->getWidth() && ly >= 0 && ly <= widget->getHeight()) {
            return widget;
        }
    }

    return nullptr;
}

Widget* Container::notContainerAt(const PointI &point) {
    auto child = widgetAt(point);

    if (auto container = dynamic_cast<Container*>(child); container != nullptr) {
        return container->notContainerAt(container->mapFromParent(point));
    } else {
        return child;
    }
}

Container* Container::containerAt(const PointI &point) {
    auto child = widgetAt(point);
    return dynamic_cast<Container*>(child);
}

void Container::onChildRemoved(Object *child) {
    requestLayout();
    invalidate();
}

void Container::onAnimate() {
    for (auto child : getChildren()) {
        if (auto widget = dynamic_cast<Widget*>(child)) {
            widget->animate();
        }
    }
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

    auto children = getWidgets();

    for (auto child : children) {
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

    for (auto child : children) {
        auto policy_v = child->getVerticalSizePolicy();
        auto policy_h = child->getHorizontalSizePolicy();

        if (child->isVisible() && (policy_v == SizePolicy::MatchParent || policy_h == SizePolicy::MatchParent)) {
            child->forceLayout();
        }
    }
}

void Container::onDraw(Painter &painter) {
    WidgetDisplayOptions options;
    options.width = getWidth();
    options.height = getHeight();
    options.paddingLeft = getPaddingLeft();
    options.paddingTop = getPaddingTop();
    options.paddingRight = getPaddingRight();
    options.paddingBottom = getPaddingBottom();
    options.framebuffer = m_framebuffer;
    options.painter = &painter;

    for (auto child : getWidgets()) {
        child->display(options);
    }
}

void Container::fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) {
    Widget::fromXML(node, io);

    for (auto child : node.children()) {
        auto childPtr = TypeRegistry::create<Widget>(child.name());
        childPtr->setParent(this);
        childPtr->fromXML(child, io);
        addWidget(childPtr);
    }
}
}
