#include <algine/core/widgets/Layout.h>
#include <algine/core/widgets/Alignment.h>

#include <stdexcept>

#include "LayoutProperties.h"

namespace algine {
inline static void requestParentUpdate(Widget *widget) {
    if (auto parent = widget->getParentWidget(); parent != nullptr) {
        parent->requestLayout();
        parent->invalidate();
    }
}

inline static void setMarginProperty(Widget *widget, const char *name, int value) {
    if (value == 0) {
        widget->removeProperty(name);
    } else {
        widget->setProperty(name, value);
    }
}

void Layout::setMargin(Widget *widget, int left, int top, int right, int bottom) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginLeft, left);
    setMarginProperty(widget, Property_MarginTop, top);
    setMarginProperty(widget, Property_MarginRight, right);
    setMarginProperty(widget, Property_MarginBottom, bottom);
}

void Layout::setMarginLeft(Widget *widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginLeft, margin);
}

void Layout::setMarginTop(Widget *widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginTop, margin);
}

void Layout::setMarginRight(Widget *widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginRight, margin);
}

void Layout::setMarginBottom(Widget *widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginBottom, margin);
}

inline static int getMarginProperty(Widget *widget, const char *name) {
    if (widget->hasProperty(name)) {
        auto &property = widget->property(name);

        if (property.type() == typeid(int)) {
            return std::any_cast<int>(property);
        } else if (property.type() == typeid(float)) {
            return static_cast<int>(std::any_cast<float>(property));
        } else {
            throw std::invalid_argument("Invalid margin type");
        }
    } else {
        return 0;
    }
}

int Layout::getMarginLeft(Widget *widget) {
    return getMarginProperty(widget, Property_MarginLeft);
}

int Layout::getMarginTop(Widget *widget) {
    return getMarginProperty(widget, Property_MarginTop);
}

int Layout::getMarginRight(Widget *widget) {
    return getMarginProperty(widget, Property_MarginRight);
}

int Layout::getMarginBottom(Widget *widget) {
    return getMarginProperty(widget, Property_MarginBottom);
}

void Layout::setAlignment(Widget *widget, uint alignment) {
    requestParentUpdate(widget);

    if (alignment == (Alignment::Top | Alignment::Left)) {
        widget->removeProperty(Property_Alignment);
    } else {
        widget->setProperty(Property_Alignment, (int) alignment);
    }
}

uint Layout::getAlignment(Widget *widget) {
    if (widget->hasProperty(Property_Alignment)) {
        if (auto &property = widget->property(Property_Alignment); property.type() == typeid(int)) {
            return std::any_cast<int>(property);
        } else if (property.type() == typeid(std::string)) {
            auto alignment = Alignment::parse(std::any_cast<std::string&>(property).c_str());
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

void Layout::setChildXDirectly(Widget *child, int x) {
    reinterpret_cast<Layout*>(child)->m_geometry.setX(x);
}

void Layout::setChildYDirectly(Widget *child, int y) {
    reinterpret_cast<Layout*>(child)->m_geometry.setY(y);
}

void Layout::childGeometryChanged(Widget *child, const RectI &geometry) {
    reinterpret_cast<Layout*>(child)->onGeometryChanged(geometry);
}
}
