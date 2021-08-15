#include <algine/core/widgets/Layout.h>
#include <algine/core/widgets/Alignment.h>

#include <stdexcept>

namespace algine {
inline void requestParentUpdate(Layout::WidgetAutoRawPtr widget) {
    if (auto parent = widget->getParent(); parent != nullptr) {
        parent->requestLayout();
        parent->invalidate();
    }
}

#define Property_MarginLeft "marginLeft"
#define Property_MarginTop "marginTop"
#define Property_MarginRight "marginRight"
#define Property_MarginBottom "marginBottom"
#define Property_Alignment "layoutAlignment"

inline void setMarginProperty(const Layout::WidgetAutoRawPtr &widget, const char *name, int value) {
    if (value == 0) {
        widget->removeProperty(name);
    } else {
        widget->setProperty(name, value);
    }
}

void Layout::setMargin(WidgetAutoRawPtr widget, int left, int top, int right, int bottom) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginLeft, left);
    setMarginProperty(widget, Property_MarginTop, top);
    setMarginProperty(widget, Property_MarginRight, right);
    setMarginProperty(widget, Property_MarginBottom, bottom);
}

void Layout::setMarginLeft(WidgetAutoRawPtr widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginLeft, margin);
}

void Layout::setMarginTop(WidgetAutoRawPtr widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginTop, margin);
}

void Layout::setMarginRight(WidgetAutoRawPtr widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginRight, margin);
}

void Layout::setMarginBottom(WidgetAutoRawPtr widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginBottom, margin);
}

inline int getMarginProperty(const Layout::WidgetAutoRawPtr &widget, const char *name) {
    if (widget->hasProperty(name)) {
        return widget->property(name).as<int>();
    } else {
        return 0;
    }
}

int Layout::getMarginLeft(WidgetAutoRawPtr widget) {
    return getMarginProperty(widget, Property_MarginLeft);
}

int Layout::getMarginTop(WidgetAutoRawPtr widget) {
    return getMarginProperty(widget, Property_MarginTop);
}

int Layout::getMarginRight(WidgetAutoRawPtr widget) {
    return getMarginProperty(widget, Property_MarginRight);
}

int Layout::getMarginBottom(WidgetAutoRawPtr widget) {
    return getMarginProperty(widget, Property_MarginBottom);
}

void Layout::setAlignment(WidgetAutoRawPtr widget, uint alignment) {
    requestParentUpdate(widget);

    if (alignment == (Alignment::Top | Alignment::Left)) {
        widget->removeProperty(Property_Alignment);
    } else {
        widget->setProperty(Property_Alignment, (int) alignment);
    }
}

uint Layout::getAlignment(WidgetAutoRawPtr widget) {
    if (widget->hasProperty(Property_Alignment)) {
        if (auto &property = widget->property(Property_Alignment); property.is<int>()) {
            return property.as<int>();
        } else if (property.is<std::string>()) {
            auto alignment = Alignment::parse(property.as<std::string>().c_str());
            property = (int) alignment;
            return alignment;
        } else {
            throw std::invalid_argument("Invalid alignment type");
        }
    } else {
        return Alignment::Top | Alignment::Left;
    }
}

/*
 * setChildXDirectly and setChildYDirectly are quite dangerous functions
 * as they don't trigger callbacks when the geometry of the Widget
 * changes, but in the other hand this is also an advantage: these
 * functions are very useful during placing children in onLayout() call,
 * as they can exclude extra measure() and onMeasure() calls.
 * Don't forget to call childGeometryChanged if the position of the child
 * has changed.
 */

void Layout::setChildXDirectly(WidgetAutoRawPtr child, int x) {
    reinterpret_cast<Layout*>(child.get())->m_geometry.setX(x);
}

void Layout::setChildYDirectly(WidgetAutoRawPtr child, int y) {
    reinterpret_cast<Layout*>(child.get())->m_geometry.setY(y);
}

void Layout::childGeometryChanged(WidgetAutoRawPtr child, const RectI &geometry) {
    reinterpret_cast<Layout *>(child.get())->onGeometryChanged(geometry);
}
}
