#ifndef ALGINE_WIDGET_H
#define ALGINE_WIDGET_H

#include <algine/core/widgets/SizePolicy.h>
#include <algine/core/widgets/WidgetPtr.h>
#include <algine/core/shader/ShaderProgramPtr.h>
#include <algine/core/texture/Texture2DPtr.h>
#include <algine/core/FramebufferPtr.h>
#include <algine/core/painter/Paint.h>
#include <algine/core/Rect.h>
#include <algine/types.h>

#include <string>
#include <variant>
#include <map>

#include <glm/mat4x4.hpp>

namespace pugi {
class xml_node;
}

namespace algine {
class Painter;
class IOSystem;

class Widget {
public:
    enum class Filtering {
        Nearest = 0x2600,
        Linear = 0x2601
    };

    struct DisplayOptions {
        int parentWidth;
        int parentHeight;
        int parentPaddingLeft {0};
        int parentPaddingTop {0};
        int parentPaddingRight {0};
        int parentPaddingBottom {0};
        Framebuffer *parentFramebuffer {nullptr};
        Painter *painter;
    };

    class Property {
    public:
        using property = std::variant<bool, int, float, std::string, char*, void*, Ptr<void>>;

    public:
        Property() = default;

        template<typename T>
        Property(T &&value);

        template<typename T>
        Property& operator=(T &&rhs);

        template<typename T>
        constexpr T& as();

        template<typename T>
        constexpr bool is() const;

        const property& get_std() const;

    private:
        property m_prop;
    };

public:
    Widget();
    virtual ~Widget() = default;

    virtual void setGeometry(RectI geometry);
    const RectI& getGeometry() const;

    void setVisible(bool visible);
    bool isVisible() const;

    void setX(int x);
    void setY(int y);
    void setWidth(int width);
    void setHeight(int height);
    void setMinWidth(int minWidth);
    void setMinHeight(int minHeight);
    void setMaxWidth(int maxWidth);
    void setMaxHeight(int maxHeight);

    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    int getMinWidth() const;
    int getMinHeight() const;
    int getMaxWidth() const;
    int getMaxHeight() const;

    void setName(const std::string &name);
    const std::string& getName() const;

    void setBackground(const Paint &paint);
    const Paint& getBackground() const;

    void setPadding(int left, int top, int right, int bottom);
    void setPaddingLeft(int padding);
    void setPaddingTop(int padding);
    void setPaddingRight(int padding);
    void setPaddingBottom(int padding);

    int getPaddingLeft() const;
    int getPaddingTop() const;
    int getPaddingRight() const;
    int getPaddingBottom() const;

    int getContentWidth() const;
    int getContentHeight() const;

    void setRotate(float degrees);
    float getRotate() const;

    void setScaleX(float scale);
    void setScaleY(float scale);
    void setScale(float scale);

    float getScaleX() const;
    float getScaleY() const;

    void setParent(Widget *parent);
    Widget* getParent() const;

    void setSizePolicy(SizePolicy horizontal, SizePolicy vertical);
    void setHorizontalSizePolicy(SizePolicy policy);
    void setVerticalSizePolicy(SizePolicy policy);

    SizePolicy getHorizontalSizePolicy() const;
    SizePolicy getVerticalSizePolicy() const;

    void setFiltering(Filtering filtering);
    Filtering getFiltering() const;

    void forceLayout();
    void requestLayout();
    void invalidate();

    void display(const DisplayOptions &options);

    void measure();
    void layout();

    void setBoundingRectPos(int x, int y, const RectI &rect);
    void setBoundingRectPos(int x, int y);

    PointI toLocalPoint(const PointI &globalPoint) const;
    RectI boundingRect() const;

    void setProperty(const char *name, const Property &property);
    const Property& getProperty(const char *name) const;
    Property& property(const char *name);
    bool hasProperty(const char *name) const;
    bool removeProperty(const char *name);

    virtual void fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io);
    bool fromXML(const std::string &xml, const std::shared_ptr<IOSystem> &io);
    bool fromXMLFile(const std::string &file, const std::shared_ptr<IOSystem> &io);
    bool fromXML(const std::string &xml);
    bool fromXMLFile(const std::string &file);

    static WidgetPtr constructFromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io);
    static WidgetPtr constructFromXML(const std::string &xml, const std::shared_ptr<IOSystem> &io);
    static WidgetPtr constructFromXMLFile(const std::string &file, const std::shared_ptr<IOSystem> &io);
    static WidgetPtr constructFromXML(const std::string &xml);
    static WidgetPtr constructFromXMLFile(const std::string &file);

protected:
    enum class Flag {
        RedrawRequired,
        SizeChanged,
        Visible,
        MeasurementRequired,
        LayoutRequired
    };

    using Flags = uint;

    void setFlag(Flag flag, bool on = true);
    bool isFlagEnabled(Flag flag) const;

protected:
    virtual int matchParentWidth(Widget *child);
    virtual int matchParentHeight(Widget *child);

    virtual void measure(int &width, int &height);
    virtual void onLayout();
    virtual void drawingStart(Painter &painter);
    virtual void drawBackground(Painter &painter);
    virtual void draw(Painter &painter) = 0;
    virtual void geometryChanged(const RectI &geometry);

protected:
    Texture2DPtr m_texture;
    FramebufferPtr m_framebuffer;

protected:
    Flags m_flags;
    RectI m_geometry;
    Widget *m_parent;
    std::string m_name;
    Paint m_background;

    int m_minWidth;
    int m_minHeight;
    int m_maxWidth;
    int m_maxHeight;

    int m_paddingLeft;
    int m_paddingTop;
    int m_paddingRight;
    int m_paddingBottom;

    float m_rotate;

    float m_scaleX;
    float m_scaleY;

    SizePolicy m_horizontalPolicy;
    SizePolicy m_verticalPolicy;

    std::map<unsigned long, Property> m_properties;
};

template<typename T>
inline Widget::Property::Property(T &&value)
    : m_prop(std::forward<T>(value)) {}

template<typename T>
inline Widget::Property& Widget::Property::operator=(T &&rhs) {
    m_prop = std::forward<T>(rhs);
    return *this;
}

template<typename T>
inline constexpr T& Widget::Property::as() {
    return std::get<T>(m_prop);
}

template<typename T>
inline constexpr bool Widget::Property::is() const {
    return std::get_if<T>(&m_prop);
}

inline const Widget::Property::property& Widget::Property::get_std() const {
    return m_prop;
}
}

#endif //ALGINE_WIDGET_H
