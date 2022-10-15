#include <algine/core/widgets/Layout.h>
#include <algine/core/widgets/Alignment.h>

#include <stdexcept>

#include "LayoutProperties.h"

namespace algine {
inline void requestParentUpdate(WidgetPtrView widget) {
    if (auto parent = widget->getParent().getWidget(); parent != nullptr) {
        parent->requestLayout();
        parent->invalidate();
    }
}

inline void setMarginProperty(const WidgetPtrView &widget, const char *name, int value) {
    if (value == 0) {
        widget->removeProperty(name);
    } else {
        widget->setProperty(name, value);
    }
}

void Layout::setMargin(WidgetPtrView widget, int left, int top, int right, int bottom) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginLeft, left);
    setMarginProperty(widget, Property_MarginTop, top);
    setMarginProperty(widget, Property_MarginRight, right);
    setMarginProperty(widget, Property_MarginBottom, bottom);
}

void Layout::setMarginLeft(WidgetPtrView widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginLeft, margin);
}

void Layout::setMarginTop(WidgetPtrView widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginTop, margin);
}

void Layout::setMarginRight(WidgetPtrView widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginRight, margin);
}

void Layout::setMarginBottom(WidgetPtrView widget, int margin) {
    requestParentUpdate(widget);
    setMarginProperty(widget, Property_MarginBottom, margin);
}

inline int getMarginProperty(const WidgetPtrView &widget, const char *name) {
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

int Layout::getMarginLeft(WidgetPtrView widget) {
    return getMarginProperty(widget, Property_MarginLeft);
}

int Layout::getMarginTop(WidgetPtrView widget) {
    return getMarginProperty(widget, Property_MarginTop);
}

int Layout::getMarginRight(WidgetPtrView widget) {
    return getMarginProperty(widget, Property_MarginRight);
}

int Layout::getMarginBottom(WidgetPtrView widget) {
    return getMarginProperty(widget, Property_MarginBottom);
}

void Layout::setAlignment(WidgetPtrView widget, uint alignment) {
    requestParentUpdate(widget);

    if (alignment == (Alignment::Top | Alignment::Left)) {
        widget->removeProperty(Property_Alignment);
    } else {
        widget->setProperty(Property_Alignment, (int) alignment);
    }
}

uint Layout::getAlignment(WidgetPtrView widget) {
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

void Layout::setChildXDirectly(WidgetPtrView child, int x) {
    reinterpret_cast<Layout*>(child.get())->m_geometry.setX(x);
}

void Layout::setChildYDirectly(WidgetPtrView child, int y) {
    reinterpret_cast<Layout*>(child.get())->m_geometry.setY(y);
}

void Layout::childGeometryChanged(WidgetPtrView child, const RectI &geometry) {
    reinterpret_cast<Layout *>(child.get())->onGeometryChanged(geometry);
}
}
