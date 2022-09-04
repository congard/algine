#include <algine/core/widgets/RelativeLayout.h>
#include <algine/core/widgets/Alignment.h>

#include <stdexcept>

#include "LayoutProperties.h"

#define Property_Above "above"
#define Property_Below "below"
#define Property_ToRightOf "toRightOf"
#define Property_ToLeftOf "toLeftOf"

#define Property_AlignLeft "alignLeft"
#define Property_AlignRight "alignRight"
#define Property_AlignTop "alignTop"
#define Property_AlignBottom "alignBottom"

namespace algine {
inline void requestParentUpdate(Layout::WidgetAutoRawPtr widget) {
    if (auto parent = widget->getParent(); parent != nullptr) {
        parent->requestLayout();
        parent->invalidate();
    }
}

inline void setPosProperty(Layout::WidgetAutoRawPtr widget, const char *name, std::string_view value) {
    if (value.empty()) {
        widget->removeProperty(name);
    } else {
        widget->setProperty(name, std::string(value));
    }
}

void RelativeLayout::setAbove(WidgetAutoRawPtr widget, std::string_view name) {
    requestParentUpdate(widget);
    setPosProperty(widget, Property_Above, name);
}

void RelativeLayout::setBelow(WidgetAutoRawPtr widget, std::string_view name) {
    requestParentUpdate(widget);
    setPosProperty(widget, Property_Below, name);
}

void RelativeLayout::setToRightOf(WidgetAutoRawPtr widget, std::string_view name) {
    requestParentUpdate(widget);
    setPosProperty(widget, Property_ToRightOf, name);
}

void RelativeLayout::setToLeftOf(WidgetAutoRawPtr widget, std::string_view name) {
    requestParentUpdate(widget);
    setPosProperty(widget, Property_ToLeftOf, name);
}

inline std::string getStringProperty(Layout::WidgetAutoRawPtr widget, const char *name) {
    if (widget->hasProperty(name)) {
        auto &property = widget->property(name);

        if (property.type() == typeid(std::string)) {
            return std::any_cast<std::string&>(property);
        } else {
            throw std::invalid_argument("Invalid type: only std::string is acceptable");
        }
    } else {
        return "";
    }
}

std::string RelativeLayout::getAbove(WidgetAutoRawPtr widget) {
    return getStringProperty(widget, Property_Above);
}

std::string RelativeLayout::getBelow(WidgetAutoRawPtr widget) {
    return getStringProperty(widget, Property_Below);
}

std::string RelativeLayout::getToRightOf(WidgetAutoRawPtr widget) {
    return getStringProperty(widget, Property_ToRightOf);
}

std::string RelativeLayout::getToLeftOf(WidgetAutoRawPtr widget) {
    return getStringProperty(widget, Property_ToLeftOf);
}

void RelativeLayout::setAlignLeft(Layout::WidgetAutoRawPtr widget, std::string_view name) {
    requestParentUpdate(widget);
    setPosProperty(widget, Property_AlignLeft, name);
}

void RelativeLayout::setAlignRight(Layout::WidgetAutoRawPtr widget, std::string_view name) {
    requestParentUpdate(widget);
    setPosProperty(widget, Property_AlignRight, name);
}

void RelativeLayout::setAlignTop(Layout::WidgetAutoRawPtr widget, std::string_view name) {
    requestParentUpdate(widget);
    setPosProperty(widget, Property_AlignTop, name);
}

void RelativeLayout::setAlignBottom(Layout::WidgetAutoRawPtr widget, std::string_view name) {
    requestParentUpdate(widget);
    setPosProperty(widget, Property_AlignBottom, name);
}

std::string RelativeLayout::getAlignLeft(Layout::WidgetAutoRawPtr widget) {
    return getStringProperty(widget, Property_AlignLeft);
}

std::string RelativeLayout::getAlignRight(Layout::WidgetAutoRawPtr widget) {
    return getStringProperty(widget, Property_AlignRight);
}

std::string RelativeLayout::getAlignTop(Layout::WidgetAutoRawPtr widget) {
    return getStringProperty(widget, Property_AlignTop);
}

std::string RelativeLayout::getAlignBottom(Layout::WidgetAutoRawPtr widget) {
    return getStringProperty(widget, Property_AlignBottom);
}

class RelativeLayout::WidgetInfo {
public:
    enum class Status {
        Placed,
        InProcess
    };

    inline WidgetInfo(Widget *widget)
        : m_widget(widget) {}

    inline operator WidgetAutoRawPtr() const { return m_widget; }

    inline void setX(int x) { m_x = x; }
    inline void setY(int y) { m_y = y; }
    inline void setStatus(Status status) { m_status = status; }
    inline void setGeometryChanged(bool changed) { m_geometryChanged = changed; }

    inline auto getWidget() const { return m_widget; }
    inline auto getStatus() const { return m_status; }

    inline auto getX() const {
        return (m_x == INT32_MIN) ? m_widget->getX() : m_x;
    }

    inline auto getY() const {
        return (m_y == INT32_MIN) ? m_widget->getY() : m_y;
    }

    inline auto getWidth() const { return m_widget->getWidth(); }
    inline auto getHeight() const { return m_widget->getHeight(); }
    inline auto isGeometryChanged() const { return m_geometryChanged; }

private:
    Widget *m_widget;
    Status m_status {Status::InProcess};
    int m_x {INT32_MIN};
    int m_y {INT32_MIN};
    bool m_geometryChanged {false};
};

RelativeLayout::WidgetInfo& RelativeLayout::layout_for(Widget *widget, std::list<WidgetInfo> &widgetsInfo) {
    if (widget == nullptr)
        throw std::invalid_argument("Widget can't be null");

    {
        auto it = [&]() {
            for (auto i = widgetsInfo.begin(); i != widgetsInfo.end(); ++i) {
                if (i->getWidget() == widget) {
                    return i;
                }
            }

            return widgetsInfo.end();
        }();

        if (it != widgetsInfo.end()) {
            if (auto status = it->getStatus(); status == WidgetInfo::Status::Placed) {
                return *it;
            } else if (status == WidgetInfo::Status::InProcess) {
                throw std::runtime_error("Can't use as base widget which hasn't been placed yet");
            }
        }
    }

    auto &widgetInfo = widgetsInfo.emplace_back(widget);

    auto getBase = [&](const char *prop) -> WidgetInfo& {
        auto base = findChild(std::any_cast<const std::string&>(widget->property(prop))).get();

        if (base == nullptr) {
            throw std::runtime_error("Base widget couldn't be found");
        }

        return layout_for(base, widgetsInfo);
    };

    PointI prevPos {widget->getX(), widget->getY()};

    if (widget->hasProperty(Property_Alignment)) {
        uint alignment = getAlignment(widget);
        uint alignment_x = alignment & 0b0011u;
        uint alignment_y = alignment & 0b1100u;

        switch (alignment_x) {
            case Alignment::Left:
                widgetInfo.setX(getMarginLeft(widget));
                break;
            case Alignment::Right:
                widgetInfo.setX(getContentWidth() - getMarginRight(widget) - widget->getWidth());
                break;
            case Alignment::Center:
                widgetInfo.setX((getContentWidth() - widget->getWidth()) / 2);
                break;
            default: throw std::invalid_argument("Invalid horizontal alignment value");
        }

        switch (alignment_y) {
            case Alignment::Top:
                widgetInfo.setY(getMarginTop(widget));
                break;
            case Alignment::Bottom:
                widgetInfo.setY(getContentHeight() - getMarginBottom(widget) - widget->getHeight());
                break;
            case Alignment::Center:
                widgetInfo.setY((getContentHeight() - widget->getHeight()) / 2);
                break;
            default: throw std::invalid_argument("Invalid vertical alignment value");
        }

        goto done;
    }

    // these properties can be used in pair with properties
    // such as alignTop, alignBottom, alignRight, alignLeft

    if (widget->hasProperty(Property_Above)) {
        auto &base = getBase(Property_Above);
        widgetInfo.setY(base.getY() - getMarginTop(base) - getMarginBottom(widget) - widget->getHeight());
        widgetInfo.setX(base.getX());
    } else if (widget->hasProperty(Property_Below)) {
        auto &base = getBase(Property_Below);
        widgetInfo.setY(base.getY() + base.getHeight() + getMarginBottom(base) + getMarginTop(widget));
        widgetInfo.setX(base.getX());
    } else if (widget->hasProperty(Property_ToRightOf)) {
        auto &base = getBase(Property_ToRightOf);
        widgetInfo.setX(base.getX() + base.getWidth() + getMarginRight(base) + getMarginLeft(widget));
        widgetInfo.setY(base.getY());
    } else if (widget->hasProperty(Property_ToLeftOf)) {
        auto &base = getBase(Property_ToLeftOf);
        widgetInfo.setX(base.getX() - getMarginLeft(base) - getMarginRight(widget) - widget->getWidth());
        widgetInfo.setY(base.getY());
    }

    // relative alignment must be performed at the very end,
    // as it can be horizontal and/or vertical and/or can be
    // used in pair with properties such as above, below,
    // toRightOf, toLeftOf

    if (widget->hasProperty(Property_AlignTop)) {
        widgetInfo.setY(getBase(Property_AlignTop).getY() + getMarginTop(widget));
    } else if (widget->hasProperty(Property_AlignBottom)) {
        auto &base = getBase(Property_AlignBottom);
        widgetInfo.setY(base.getY() + base.getHeight() - widget->getHeight() - getMarginBottom(widget));
    }

    if (widget->hasProperty(Property_AlignRight)) {
        auto &base = getBase(Property_AlignRight);
        widgetInfo.setX(base.getX() + base.getWidth() - widget->getWidth() - getMarginRight(widget));
    } else if (widget->hasProperty(Property_AlignLeft)) {
        widgetInfo.setX(getBase(Property_AlignLeft).getX() + getMarginLeft(widget));
    }

    done: {
        widgetInfo.setStatus(WidgetInfo::Status::Placed);

        if (prevPos != PointI {widgetInfo.getX(), widgetInfo.getY()}) {
            widgetInfo.setGeometryChanged(true);
        }

        return widgetInfo;
    }
}

void RelativeLayout::onMeasure(int &width, int &height) {
    Widget::onMeasure(width, height);

    if (m_horizontalPolicy != SizePolicy::Preferred && m_verticalPolicy != SizePolicy::Preferred)
        return;

    auto keepMax = [](int &in, int p) {
        if (in < p) {
            in = p;
        }
    };

    std::list<WidgetInfo> widgetsInfo;

    int maxX = INT32_MIN;
    int maxY = INT32_MIN;

    for (auto &child : m_children) {
        child->measure();

        auto &childInfo = layout_for(child.get(), widgetsInfo);

        keepMax(maxX, childInfo.getX() + childInfo.getWidth() + getMarginRight(child));
        keepMax(maxY, childInfo.getY() + childInfo.getHeight() + getMarginBottom(child));
    }

    if (m_horizontalPolicy == SizePolicy::Preferred) {
        width = maxX;
    }

    if (m_verticalPolicy == SizePolicy::Preferred) {
        height = maxY;
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

void RelativeLayout::onLayout() {
    std::list<WidgetInfo> widgetsInfo;

    for (auto &child : m_children) {
        child->measure();
        child->layout();

        auto &childInfo = layout_for(child.get(), widgetsInfo);

        if (childInfo.isGeometryChanged()) {
            setChildXDirectly(child, childInfo.getX());
            setChildYDirectly(child, childInfo.getY());
            childGeometryChanged(child, child->getGeometry());
        }
    }
}
}
