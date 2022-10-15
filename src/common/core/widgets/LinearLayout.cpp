#include <algine/core/widgets/LinearLayout.h>
#include <algine/core/widgets/Alignment.h>
#include <algine/core/log/Log.h>

#include <pugixml.hpp>

#include <cstring>

namespace algine {
void LinearLayout::setOrientation(Orientation orientation) {
    if (orientation != m_orientation) {
        requestLayout();
        invalidate();
        m_orientation = orientation;
    }
}

LinearLayout::Orientation LinearLayout::getOrientation() const {
    return m_orientation;
}

int LinearLayout::matchParentWidth(Widget *child) {
    return getContentWidth() - Layout::getMarginLeft(child) - Layout::getMarginRight(child);
}

int LinearLayout::matchParentHeight(Widget *child) {
    return getContentHeight() - Layout::getMarginTop(child) - Layout::getMarginBottom(child);
}

void LinearLayout::onMeasure(int &width, int &height) {
    Widget::onMeasure(width, height);

    if (m_horizontalPolicy != SizePolicy::Preferred && m_verticalPolicy != SizePolicy::Preferred)
        return;

    if (m_orientation == Orientation::Vertical) {
        measure_vertical_preferred(width, height);
    } else {
        measure_horizontal_preferred(width, height);
    }

    setMeasuredDimension(width, height);

    for (auto &child : m_children) {
        auto policy_v = child->getVerticalSizePolicy();
        auto policy_h = child->getHorizontalSizePolicy();

        if (child->isVisible() && (policy_v == SizePolicy::MatchParent || policy_h == SizePolicy::MatchParent)) {
            child->forceLayout();
        }
    }
}

void LinearLayout::onLayout() {
    int pos = 0;

    void (LinearLayout::*placeChild)(const WidgetPtr&, int&);

    if (m_orientation == Orientation::Vertical) {
        placeChild = &LinearLayout::place_vertically;
        pos = getPaddingTop();
    } else {
        placeChild = &LinearLayout::place_horizontally;
        pos = getPaddingLeft();
    }

    for (auto &child : m_children) {
        if (!child->isVisible())
            continue;

        child->measure();
        child->layout();

        PointI prevPos {child->getX(), child->getY()};

        // place child based on orientation of LinearLayout
        (this->*placeChild)(child, pos);

        if (prevPos != PointI {child->getX(), child->getY()}) {
            childGeometryChanged(child, child->getGeometry());
        }
    }
}

void LinearLayout::place_vertically(const WidgetPtr &child, int &y) {
    y += Layout::getMarginTop(child);
    setChildYDirectly(child, y);
    y += child->getHeight();
    y += Layout::getMarginBottom(child);

    // In vertical mode, the horizontal alignment of the child is ignored
    uint alignment_x = Layout::getAlignment(child) & 0b0011u;

    switch (alignment_x) {
        case Alignment::Center:
            setChildXDirectly(child, getPaddingLeft() + (getContentWidth() - child->getWidth()) / 2);
            break;
        case Alignment::Left:
            setChildXDirectly(child, getPaddingLeft() + Layout::getMarginLeft(child));
            break;
        case Alignment::Right:
            setChildXDirectly(child, getPaddingLeft() + getContentWidth() - Layout::getMarginRight(child) - child->getWidth());
            break;
        default: throw std::invalid_argument("Invalid horizontal alignment value");
    }
}

void LinearLayout::place_horizontally(const WidgetPtr &child, int &x) {
    x += Layout::getMarginLeft(child);
    setChildXDirectly(child, x);
    x += child->getWidth();
    x += Layout::getMarginRight(child);

    // In horizontal mode, the vertical alignment of the child is ignored
    uint alignment_y = Layout::getAlignment(child) & 0b1100u;

    switch (alignment_y) {
        case Alignment::Center:
            setChildYDirectly(child, getPaddingTop() + (getContentHeight() - child->getHeight()) / 2);
            break;
        case Alignment::Top:
            setChildYDirectly(child, getPaddingTop() + Layout::getMarginTop(child));
            break;
        case Alignment::Bottom:
            setChildYDirectly(child, getPaddingTop() + getContentHeight() - Layout::getMarginBottom(child) - child->getHeight());
            break;
        default: throw std::invalid_argument("Invalid vertical alignment value");
    }
}

void LinearLayout::measure_vertical_preferred(int &width, int &height) {
    int y = 0;
    int maxWidth = 0;

    for (auto &child : m_children) {
        if (!child->isVisible())
            continue;

        child->measure();

        y += Layout::getMarginTop(child) + child->getHeight() + Layout::getMarginBottom(child);

        auto childRequiredWidth = Layout::getMarginLeft(child) + child->getWidth() + Layout::getMarginRight(child);

        if (childRequiredWidth > maxWidth) {
            maxWidth = childRequiredWidth;
        }
    }

    if (m_horizontalPolicy == SizePolicy::Preferred) {
        width = maxWidth;
    }

    if (m_verticalPolicy == SizePolicy::Preferred) {
        height = y;
    }
}

void LinearLayout::measure_horizontal_preferred(int &width, int &height) {
    int x = 0;
    int maxHeight = 0;

    for (auto &child : m_children) {
        if (!child->isVisible())
            continue;

        child->measure();

        x += Layout::getMarginLeft(child) + child->getWidth() + Layout::getMarginRight(child);

        auto childRequiredHeight = Layout::getMarginTop(child) + child->getHeight() + Layout::getMarginBottom(child);

        if (childRequiredHeight > maxHeight) {
            maxHeight = childRequiredHeight;
        }
    }

    if (m_horizontalPolicy == SizePolicy::Preferred) {
        width = x;
    }

    if (m_verticalPolicy == SizePolicy::Preferred) {
        height = maxHeight;
    }
}

void LinearLayout::fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) {
    Layout::fromXML(node, io);

    for (pugi::xml_attribute attr : node.attributes()) {
        if (strcmp(attr.name(), "orientation") == 0) {
            if (auto value = attr.value(); strcmp(value, "vertical") == 0) {
                setOrientation(Orientation::Vertical);
            } else if (strcmp(value, "horizontal") == 0) {
                setOrientation(Orientation::Horizontal);
            } else {
                throw std::invalid_argument("Invalid orientation value '" + std::string(value) + "'");
            }
        }
    }
}
}
