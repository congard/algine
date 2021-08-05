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

#include <glm/mat4x4.hpp>

namespace pugi {
class xml_node;
}

namespace algine {
class Painter;
class IOSystem;

class Widget {
    friend class Container;

public:
    enum class Flag {
        RedrawRequired = 0b0001,
        SizeChanged = 0b0010,
        Visible = 0b0100
    };

    using Flags = uint;

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

public:
    Widget();
    virtual ~Widget() = default;

    virtual void setGeometry(RectI geometry);
    const RectI& getGeometry() const;

    void setFlag(Flag flag, bool on = true);
    bool isFlagEnabled(Flag flag) const;
    void setFlags(Flags flags);
    Flags getFlags() const;

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

    void invalidate();

    void display(const DisplayOptions &options);

    PointI toLocalPoint(const PointI &globalPoint) const;
    RectI boundingRect() const;

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
    virtual void measure(int &width, int &height);
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
};
}

#endif //ALGINE_WIDGET_H
