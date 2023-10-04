#ifndef ALGINE_WIDGET_H
#define ALGINE_WIDGET_H

#include <algine/core/widgets/SizePolicy.h>
#include <algine/core/widgets/WidgetDisplayOptions.h>
#include <algine/core/widgets/Layer.h>
#include <algine/core/widgets/animation/Animation.h>
#include <algine/core/unified/Event.h>
#include <algine/core/painter/Paint.h>
#include <algine/core/math/Rect.h>
#include <algine/core/PtrView.h>
#include <algine/core/Variant.h>
#include <algine/core/lua/Lua.h>
#include <algine/core/io/IOSystemPtr.h>
#include <algine/core/Object.h>
#include <algine/types.h>

#include <string>
#include <any>
#include <map>

#include <glm/mat4x4.hpp>

namespace pugi {
class xml_node;
}

namespace algine {
class IOSystem;

class AL_EXPORT Widget: public Object {
public:
    enum class Filtering {
        Nearest = 0x2600,
        Linear = 0x2601
    };

    using Property = std::any;
    using Properties = std::map<unsigned long, Property>;

    using EventListener = std::function<void(Widget*, const Event&)>;

    using Animations = std::forward_list<Widgets::AnimationPtr>;

public:
    // TODO: add Object* as a param (and to all derived classes too)
    Widget();
    ~Widget() override = default;

    /**
     * Sets widget's geometry
     * @note this function will not change widget's size policy,
     * unlike setWidth / setHeight
     * @see setX, setY, setWidth, setHeight
     * <!-- #solgen #prop_name geometry -->
     */
    virtual void setGeometry(RectI geometry);

    /// <!-- #solgen #prop_name geometry -->
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

    void setOpacity(float opacity);
    float getOpacity() const;

    Widget* getParentWidget() const;

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

    void display(const WidgetDisplayOptions &options);

    void animate();

    void measure();
    void layout();

    void setBoundingRectPos(int x, int y, const RectI &rect);
    void setBoundingRectPos(int x, int y);

    /**
     * Translates the widget coordinate <code>point</code> from the coordinate system
     * of <code>parent</code> to this widget's coordinate system.
     * <br>The <code>parent</code> must be a parent of the calling widget.
     * @param point
     * @param parent
     * @return point in this widget's coordinate system
     * @note if <code>parent</code> is <code>nullptr</code>,
     * <code>point</code> will be treated as global
     * @see mapFromParent, mapFromGlobal
     */
    PointI mapFrom(Widget *parent, const PointI &point) const;

    /**
     * The same as <code>mapFrom(getParent(), point)</code>
     * @param point
     * @return point in this widget's coordinate system
     * @see mapFrom
     */
    PointI mapFromParent(const PointI &point) const;

    /**
     * The same as <code>mapFrom(nullptr, globalPoint)</code>
     * @param globalPoint
     * @return point in this widget's coordinate system
     * @see mapFrom
     */
    PointI mapFromGlobal(const PointI &globalPoint) const;

    /**
     * Translates the widget coordinate <code>pos</code> to the coordinate system of <code>parent<code>.
     * <br>The <code>parent</code> must be a parent of the calling widget.
     * @param parent
     * @param point
     * @return point transformed from <code>this</code> to <code>parent</code> coordinate system
     * @note if <code>parent</code> is <code>nullptr</code>,
     * <code>point</code> will be treated as global
     * @see mapToParent, mapToGlobal
     */
    PointI mapTo(Widget *parent, const PointI &point) const;

    /**
     * The same as <code>mapTo(getParent(), point)</code>
     * @param point
     * @return point transformed from <code>this</code> to the parent coordinate system
     * @see mapTo
     */
    PointI mapToParent(const PointI &point) const;

    /**
     * The same as <code>mapTo(nullptr, globalPoint)</code>
     * @param globalPoint
     * @return point transformed from <code>this</code> to global coordinate system
     * @see mapTo
     */
    PointI mapToGlobal(const PointI &globalPoint) const;

    RectI boundingRect() const;

    void setProperty(std::string_view name, const Property &property);
    const Property& getProperty(std::string_view name) const;
    Property& property(std::string_view name);
    Properties::const_iterator findProperty(std::string_view name) const;
    Properties::iterator findProperty(std::string_view name);
    Properties::const_iterator propertiesEnd() const;
    Properties::iterator propertiesEnd();
    bool hasProperty(std::string_view name) const;
    bool removeProperty(std::string_view name);

    template<typename T>
    T getProperty(std::string_view name) const {
        return std::any_cast<T>(getProperty(name));
    }

    template<typename T>
    T property(std::string_view name) {
        return std::any_cast<T>(property(name));
    }

    void setEventListener(Event::Id event, const EventListener &listener);

    void setEventAnimation(Event::Id event, Widgets::AnimationPtr animation);
    bool hasAnimation(Event::Id event) const;
    const Widgets::AnimationPtr& getAnimation(Event::Id event) const;

    void addAnimation(Widgets::AnimationPtr animation);
    const Animations& getAnimations() const;

    void event(const Event &event);

    void addScript(const std::string &path, Lua *lua = nullptr);
    void addScriptSource(const std::string &source, Lua *lua = nullptr);

    /**
     * @return Lua environment associated with this widget; if it isn't valid,
     * a valid one will be created
     */
    sol::environment env(Lua *lua = nullptr);

    /**
     * @return Lua environment associated with this widget
     */
    sol::environment getEnv() const;

    virtual void fromXML(const pugi::xml_node &node, const IOSystemPtr &io);
    bool fromXML(const std::string &xml, const IOSystemPtr &io);
    bool fromXMLFile(const std::string &file, const IOSystemPtr &io);
    bool fromXML(const std::string &xml);
    bool fromXMLFile(const std::string &file);

    static Widget* constructFromXML(const pugi::xml_node &node, const IOSystemPtr &io, Object *parent = defaultParent());
    static Widget* constructFromXML(const std::string &xml, const IOSystemPtr &io, Object *parent = defaultParent());
    static Widget* constructFromXMLFile(const std::string &file, const IOSystemPtr &io, Object *parent = defaultParent());
    static Widget* constructFromXML(const std::string &xml, Object *parent = defaultParent());
    static Widget* constructFromXMLFile(const std::string &file, Object *parent = defaultParent());

    template<typename T>
    static T constructFromXML(const std::string &xml, Object *parent = defaultParent()) {
        return dynamic_cast<T>(constructFromXML(xml, parent));
    }

    template<typename T>
    static T constructFromXMLFile(const std::string &file, Object *parent = defaultParent()) {
        return dynamic_cast<T>(constructFromXMLFile(file, parent));
    }

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

    virtual void setMeasuredDimension(int width, int height);

    virtual void onAnimate();
    virtual void onMeasure(int &width, int &height);
    virtual void onLayout();
    virtual void draw(Painter &painter);
    virtual void onDrawBackground(Painter &painter);
    virtual void onDraw(Painter &painter) = 0;
    virtual void onGeometryChanged(const RectI &geometry);

protected:
    static Filtering parseFiltering(std::string_view str);

    static std::string getString(const char *str);
    float getDimenPxF(const char *str);
    int getDimenPx(const char *str);
    static Color getColor(const char *str);

protected:
    Texture2D *m_texture;
    Framebuffer *m_framebuffer;

protected:
    Flags m_flags;
    RectI m_geometry;
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

    float m_opacity;

    SizePolicy m_horizontalPolicy;
    SizePolicy m_verticalPolicy;

    Properties m_properties;

    std::unordered_map<Event::Id, EventListener> m_eventListeners;
    std::unordered_map<Event::Id, Widgets::AnimationPtr> m_eventAnimations;

    Animations m_animations;

    sol::environment m_luaEnv;

private:
    void addLuaScript(const std::string &s, bool path, Lua *lua);
};
}

#endif //ALGINE_WIDGET_H
