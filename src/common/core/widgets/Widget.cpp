#include <algine/core/widgets/Widget.h>
#include <algine/core/widgets/Units.h>
#include <algine/core/painter/Painter.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/Engine.h>
#include <algine/core/log/Log.h>
#include <algine/core/TypeRegistry.h>
#include <algine/core/Resources.h>

#include <tulz/StringUtils.h>
#include <glm/ext/matrix_transform.hpp>

#include <pugixml.hpp>

#include <cstring>

#include "core/djb2.h"
#include "TexturePathLoader.h"

#define requireParentRedraw() if (auto parent = m_parent.getWidget(); parent) parent->invalidate()
#define requireParentLayout() if (auto parent = m_parent.getWidget(); parent) parent->requestLayout()

#define continue_if(condition) if (!(condition)) return

//#define ALGINE_WIDGET_TRACE_CALLS

#ifdef ALGINE_WIDGET_TRACE_CALLS
#define print_call(str) Log::info("Widget") << (str) << Log::end
#else
#define print_call(str)
#endif

namespace algine {
Widget* Widget::Parent::getWidget() const {
    return is<Widget*>() ? as<Widget*>() : nullptr;
}

Widgets::Layer* Widget::Parent::getLayer() const {
    return is<Widgets::Layer*>() ? as<Widgets::Layer*>() : nullptr;
}

bool Widget::Parent::isNull() const {
    return getWidget() && getLayer();
}

Widget::Widget()
    : m_flags(),
      m_geometry(0, 0, 128, 128),
      m_parent(),
      m_minWidth(),
      m_minHeight(),
      m_maxWidth(),
      m_maxHeight(),
      m_paddingLeft(0),
      m_paddingTop(0),
      m_paddingRight(0),
      m_paddingBottom(0),
      m_rotate(0.0f),
      m_scaleX(1.0f),
      m_scaleY(1.0f),
      m_opacity(1.0f),
      m_horizontalPolicy(SizePolicy::Preferred),
      m_verticalPolicy(SizePolicy::Preferred),
      m_luaEnv()
{
    m_texture = PtrMaker::make();
    m_texture->bind();
    m_texture->setFormat(Texture::RGBA8);
    m_texture->setDimensions(getWidth(), getHeight());
    m_texture->setParams(std::map<uint, uint> {
        {Texture::MinFilter, Texture::Nearest},
        {Texture::MagFilter, Texture::Nearest},
        {Texture::WrapU, Texture::ClampToEdge},
        {Texture::WrapV, Texture::ClampToEdge}
    });
    m_texture->update();

    m_framebuffer = PtrMaker::make();
    m_framebuffer->bind();
    m_framebuffer->attachTexture(m_texture, Framebuffer::ColorAttachmentZero);

    setFlag(Flag::SizeChanged);

    setVisible(true);

    requestLayout();
    invalidate();
}

void Widget::setGeometry(RectI geometry) {
    if (auto w = geometry.getWidth(); w < m_minWidth) {
        geometry.setWidth(m_minWidth);
    } else if (w > m_maxWidth && m_maxWidth > 0) {
        geometry.setWidth(m_maxWidth);
    }

    if (auto h = geometry.getHeight(); h < m_minHeight) {
        geometry.setHeight(m_minHeight);
    } else if (h > m_maxHeight && m_maxHeight > 0) {
        geometry.setHeight(m_maxHeight);
    }

    if (m_geometry != geometry) {
        if (geometry.getWidth() != getWidth() || geometry.getHeight() != getHeight()) {
            setFlag(Flag::SizeChanged);
            requestLayout();
            invalidate();
        }

        if (geometry.getX() != getX() || geometry.getY() != getY()) {
            requireParentRedraw();
        }

        onGeometryChanged(geometry);
        requireParentLayout();

        m_geometry = geometry;
    }
}

const RectI& Widget::getGeometry() const {
    return m_geometry;
}

void Widget::setVisible(bool visible) {
    continue_if(visible != isFlagEnabled(Flag::Visible));
    requireParentLayout();
    requireParentRedraw();
    setFlag(Flag::Visible, visible);
}

bool Widget::isVisible() const {
    return isFlagEnabled(Flag::Visible);
}

void Widget::setX(int x) {
    setGeometry({x, m_geometry.getY(), m_geometry.getWidth(), m_geometry.getHeight()});
}

void Widget::setY(int y) {
    setGeometry({m_geometry.getX(), y, m_geometry.getWidth(), m_geometry.getHeight()});
}

void Widget::setWidth(int width) {
    if (width != getWidth())
        setHorizontalSizePolicy(SizePolicy::Fixed);
    setGeometry({m_geometry.getX(), m_geometry.getY(), width, m_geometry.getHeight()});
}

void Widget::setHeight(int height) {
    if (height != getHeight())
        setVerticalSizePolicy(SizePolicy::Fixed);
    setGeometry({m_geometry.getX(), m_geometry.getY(), m_geometry.getWidth(), height});
}

void Widget::setMinWidth(int minWidth) {
    m_minWidth = minWidth;
    setGeometry(m_geometry);
}

void Widget::setMinHeight(int minHeight) {
    m_minHeight = minHeight;
    setGeometry(m_geometry);
}

void Widget::setMaxWidth(int maxWidth) {
    m_maxWidth = maxWidth;
    setGeometry(m_geometry);
}

void Widget::setMaxHeight(int maxHeight) {
    m_maxHeight = maxHeight;
    setGeometry(m_geometry);
}

int Widget::getX() const {
    return m_geometry.getX();
}

int Widget::getY() const {
    return m_geometry.getY();
}

int Widget::getWidth() const {
    return m_geometry.getWidth();
}

int Widget::getHeight() const {
    return m_geometry.getHeight();
}

int Widget::getMinWidth() const {
    return m_minWidth;
}

int Widget::getMinHeight() const {
    return m_minHeight;
}

int Widget::getMaxWidth() const {
    return m_maxWidth;
}

int Widget::getMaxHeight() const {
    return m_maxHeight;
}

void Widget::setName(const std::string &name) {
    m_name = name;
}

const std::string& Widget::getName() const {
    return m_name;
}

void Widget::setBackground(const Paint &paint) {
    invalidate();
    m_background = paint;
}

const Paint& Widget::getBackground() const {
    return m_background;
}

void Widget::setPadding(int left, int top, int right, int bottom) {
    continue_if(left != m_paddingLeft || top != m_paddingTop || right != m_paddingRight || bottom != m_paddingBottom);
    requestLayout();
    invalidate();
    m_paddingLeft = left;
    m_paddingTop = top;
    m_paddingRight = right;
    m_paddingBottom = bottom;
}

void Widget::setPaddingLeft(int padding) {
    continue_if(padding != m_paddingLeft);
    requestLayout();
    invalidate();
    m_paddingLeft = padding;
}

void Widget::setPaddingTop(int padding) {
    continue_if(padding != m_paddingTop);
    requestLayout();
    invalidate();
    m_paddingTop = padding;
}

void Widget::setPaddingRight(int padding) {
    continue_if(padding != m_paddingRight);
    requestLayout();
    invalidate();
    m_paddingRight = padding;
}

void Widget::setPaddingBottom(int padding) {
    continue_if(padding != m_paddingBottom);
    requestLayout();
    invalidate();
    m_paddingBottom = padding;
}

int Widget::getPaddingLeft() const {
    return m_paddingLeft;
}

int Widget::getPaddingTop() const {
    return m_paddingTop;
}

int Widget::getPaddingRight() const {
    return m_paddingRight;
}

int Widget::getPaddingBottom() const {
    return m_paddingBottom;
}

int Widget::getContentWidth() const {
    return getWidth() - m_paddingLeft - m_paddingRight;
}

int Widget::getContentHeight() const {
    return getHeight() - m_paddingTop - m_paddingBottom;
}

void Widget::setRotate(float degrees) {
    continue_if(degrees != m_rotate);
    requireParentRedraw();
    m_rotate = degrees;
}

float Widget::getRotate() const {
    return m_rotate;
}

void Widget::setScaleX(float scale) {
    continue_if(scale != m_scaleX);
    requireParentRedraw();
    m_scaleX = scale;
}

void Widget::setScaleY(float scale) {
    continue_if(scale != m_scaleY);
    requireParentRedraw();
    m_scaleY = scale;
}

void Widget::setScale(float scale) {
    continue_if(scale != m_scaleX || scale != m_scaleY);
    requireParentRedraw();
    m_scaleX = scale;
    m_scaleY = scale;
}

float Widget::getScaleX() const {
    return m_scaleX;
}

float Widget::getScaleY() const {
    return m_scaleY;
}

void Widget::setOpacity(float opacity) {
    continue_if(opacity != m_opacity);
    assert(opacity >= 0.0f && opacity <= 1.0f);
    requireParentRedraw();
    m_opacity = opacity;
}

float Widget::getOpacity() const {
    return m_opacity;
}

void Widget::setParent(Parent parent) {
    std::swap(m_parent, parent);
}

Widget::Parent Widget::getParent() const {
    return m_parent;
}

void Widget::setSizePolicy(SizePolicy horizontal, SizePolicy vertical) {
    continue_if(horizontal != m_horizontalPolicy || vertical != m_verticalPolicy);
    requestLayout();
    invalidate();
    m_horizontalPolicy = horizontal;
    m_verticalPolicy = vertical;
}

void Widget::setHorizontalSizePolicy(SizePolicy policy) {
    continue_if(policy != m_horizontalPolicy);
    requestLayout();
    invalidate();
    m_horizontalPolicy = policy;
}

void Widget::setVerticalSizePolicy(SizePolicy policy) {
    continue_if(policy != m_verticalPolicy);
    requestLayout();
    invalidate();
    m_verticalPolicy = policy;
}

SizePolicy Widget::getHorizontalSizePolicy() const {
    return m_horizontalPolicy;
}

SizePolicy Widget::getVerticalSizePolicy() const {
    return m_verticalPolicy;
}

void Widget::setFiltering(Filtering filtering) {
    requireParentRedraw();

    const uint filtering_integer = static_cast<uint>(filtering);

    m_texture->bind();
    m_texture->setParams(std::map<uint, uint> {
        {Texture::MinFilter, filtering_integer},
        {Texture::MagFilter, filtering_integer}
    });
}

Widget::Filtering Widget::getFiltering() const {
    m_texture->bind();
    return static_cast<Filtering>(m_texture->getParam(Texture::MagFilter));
}

void Widget::forceLayout() {
    setFlag(Flag::MeasurementRequired);
    setFlag(Flag::LayoutRequired);
    measure();
    layout();
}

void Widget::requestLayout() {
    setFlag(Flag::MeasurementRequired);
    setFlag(Flag::LayoutRequired);
    requireParentLayout();
}

void Widget::invalidate() {
    setFlag(Flag::RedrawRequired);
    requireParentRedraw();
}

void Widget::display(const WidgetDisplayOptions &options) {
    animate();

    if (!isVisible() || m_opacity == 0.0f)
        return;

    auto painter = options.painter;

    measure();
    layout();

    if (getWidth() == 0 || getHeight() == 0)
        return;

    if (isFlagEnabled(Flag::RedrawRequired)) {
        setFlag(Flag::RedrawRequired, false);

        draw(*painter);
        onDraw(*painter);
    }

    if (options.framebuffer) {
        options.framebuffer->bind();
    } else {
        Engine::defaultFramebuffer()->bind();
    }

    int parentContentWidth = options.width - options.paddingLeft - options.paddingRight;
    int parentContentHeight = options.height - options.paddingTop - options.paddingBottom;

    Engine::setViewport(options.paddingLeft, options.paddingBottom,
                        parentContentWidth, parentContentHeight);
    painter->setViewport(parentContentWidth, parentContentHeight);

    painter->setTransform(glm::mat4(1.0f));

    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // TODO: move to Engine

    auto width = static_cast<float>(getWidth());
    auto height = static_cast<float>(getHeight());
    auto x = static_cast<float>(getX() - options.paddingLeft);
    auto y = static_cast<float>(getY() - options.paddingTop);

    painter->setOpacity(m_opacity);

    if (m_rotate == 0.0f && m_scaleX == 1.0f && m_scaleY == 1.0f) {
        painter->drawTexture(m_texture, {x, y, width, height});
    } else {
        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), {m_scaleX, m_scaleY, 1.0f});
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotate), {0.0f, 0.0f, 1.0f});
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), {x + width / 2, y + height / 2, 0.0f});

        painter->setTransform(translation * rotation * scaling);
        painter->drawTexture(m_texture, {-width / 2, -height / 2, width, height});
    }
}

void Widget::animate() {
    for (auto &p : m_eventAnimations)
        p.second->animate();

    auto prevIt = m_animations.before_begin();

    for (auto it = m_animations.begin(); it != m_animations.end();) {
        auto &animation = *it;
        animation->animate();

        if (!animation->isActive()) {
            it = m_animations.erase_after(prevIt);
        } else {
            ++it;
            ++prevIt;
        }
    }

    onAnimate();
}

void Widget::measure() {
    if (!isFlagEnabled(Flag::MeasurementRequired))
        return;

    print_call("measure(): " + getName());

    int width = getContentWidth();
    int height = getContentHeight();

    // calculate content width/height
    onMeasure(width, height);

    setMeasuredDimension(width, height);

    setFlag(Flag::MeasurementRequired, false);
}

void Widget::layout() {
    if (!isFlagEnabled(Flag::LayoutRequired))
        return;

    print_call("layout(): " + getName());

    onLayout();

    setFlag(Flag::LayoutRequired, false);
}

void Widget::setBoundingRectPos(int x, int y, const RectI &rect) {
    // 1. Move bounding rect to (0, 0)
    // 2. Center the widget in it so the origin of the bounding rect
    //    and the origin of the widget are the same
    // 3. Translate on (x, y)

    setGeometry({
        x + (rect.getWidth() - getWidth()) / 2,
        y + (rect.getHeight() - getHeight()) / 2,
        getWidth(),
        getHeight()
    });
}

void Widget::setBoundingRectPos(int x, int y) {
    setBoundingRectPos(x, y, boundingRect());
}

PointI Widget::mapFrom(WidgetPtrView parent, const PointI &point) const {
    if (parent == this)
        return point;

    Widget* thisParent = m_parent;

    if (thisParent == nullptr && parent != nullptr)
        throw std::runtime_error("The parent must be a parent of the calling widget");

    PointI localPoint = point;

    if (thisParent && parent != thisParent)
        localPoint = thisParent->mapFrom(parent, localPoint);

    localPoint -= {getX(), getY()};

    if (m_rotate != 0.0f || m_scaleX != 1.0f || m_scaleY != 1.0f) {
        auto width = getWidth();
        auto height = getHeight();

        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), {m_scaleX, m_scaleY, 1.0f});
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotate), {0.0f, 0.0f, 1.0f});

        glm::vec4 p_centered {localPoint.getX() - width / 2, localPoint.getY() - height / 2, 0.0f, 1.0f};
        glm::vec4 p = glm::inverse(rotation * scaling) * p_centered + glm::vec4(width / 2, height / 2, 0.0f, 0.0f);

        return {(int) p.x, (int) p.y};
    }

    return localPoint;
}

PointI Widget::mapFromParent(const PointI &point) const {
    return mapFrom(m_parent, point);
}

PointI Widget::mapFromGlobal(const PointI &globalPoint) const {
    return mapFrom(nullptr, globalPoint);
}

PointI Widget::mapTo(WidgetPtrView parent, const PointI &point) const {
    if (parent == this)
        return point;

    Widget* thisParent = m_parent;

    if (thisParent == nullptr && parent != nullptr)
        throw std::runtime_error("The parent must be a parent of the calling widget");

    // this -> this without transformations coordinate system

    PointI mappedPoint = point;

    if (m_rotate != 0.0f || m_scaleX != 1.0f || m_scaleY != 1.0f) {
        auto width = getWidth();
        auto height = getHeight();

        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), {m_scaleX, m_scaleY, 1.0f});
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotate), {0.0f, 0.0f, 1.0f});

        glm::vec4 p_centered {mappedPoint.getX() - width / 2, mappedPoint.getY() - height / 2, 0.0f, 1.0f};
        glm::vec4 p = rotation * scaling * p_centered + glm::vec4(width / 2, height / 2, 0.0f, 0.0f);

        mappedPoint = {(int) p.x, (int) p.y};
    }

    // this without transformations -> m_parent coordinate system
    mappedPoint += {getX(), getY()};

    if (thisParent)
        mappedPoint = thisParent->mapTo(parent, mappedPoint);

    return mappedPoint;
}

PointI Widget::mapToParent(const PointI &point) const {
    return mapTo(m_parent, point);
}

PointI Widget::mapToGlobal(const PointI &globalPoint) const {
    return mapTo(nullptr, globalPoint);
}

RectI Widget::boundingRect() const {
    if (m_rotate == 0.0f && m_scaleX == 1.0f && m_scaleY == 1.0f) {
        return m_geometry;
    }

    auto width = static_cast<float>(getWidth());
    auto height = static_cast<float>(getHeight());
    auto x = static_cast<float>(getX());
    auto y = static_cast<float>(getY());

    glm::vec4 p1 {-width / 2.0f,  height / 2.0f, 0.0f, 1.0f};
    glm::vec4 p2 { width / 2.0f,  height / 2.0f, 0.0f, 1.0f};
    glm::vec4 p3 { width / 2.0f, -height / 2.0f, 0.0f, 1.0f};
    glm::vec4 p4 {-width / 2.0f, -height / 2.0f, 0.0f, 1.0f};

    glm::mat4 scaling = glm::scale(glm::mat4(1.0f), {m_scaleX, m_scaleY, 1.0f});
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotate), {0.0f, 0.0f, 1.0f});
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), {x + width / 2, y + height / 2, 0.0f});

    glm::mat4 transform = translation * rotation * scaling;

    p1 = transform * p1;
    p2 = transform * p2;
    p3 = transform * p3;
    p4 = transform * p4;

    auto min = [](float p1, float p2) { return glm::min(p1, p2); };
    auto max = [](float p1, float p2) { return glm::max(p1, p2); };

    float minX = min(p1.x, min(p2.x, min(p3.x, p4.x)));
    float minY = min(p1.y, min(p2.y, min(p3.y, p4.y)));
    float maxX = max(p1.x, max(p2.x, max(p3.x, p4.x)));
    float maxY = max(p1.y, max(p2.y, max(p3.y, p4.y)));

    return {
        static_cast<int>(minX),
        static_cast<int>(minY),
        static_cast<int>(maxX - minX),
        static_cast<int>(maxY - minY)
    };
}

void Widget::setProperty(std::string_view name, const Property &property) {
    auto hash = hash_djb2((unsigned char *) name.data());
    m_properties[hash] = property;
}

const Widget::Property& Widget::getProperty(std::string_view name) const {
    auto hash = hash_djb2((unsigned char *) name.data());
    return m_properties.at(hash);
}

Widget::Property& Widget::property(std::string_view name) {
    auto hash = hash_djb2((unsigned char *) name.data());
    return m_properties[hash];
}

Widget::Properties::const_iterator Widget::findProperty(std::string_view name) const {
    auto hash = hash_djb2((unsigned char *) name.data());
    return m_properties.find(hash);
}

Widget::Properties::iterator Widget::findProperty(std::string_view name) {
    auto hash = hash_djb2((unsigned char *) name.data());
    return m_properties.find(hash);
}

Widget::Properties::const_iterator Widget::propertiesEnd() const {
    return m_properties.end();
}

Widget::Properties::iterator Widget::propertiesEnd() {
    return m_properties.end();
}

bool Widget::hasProperty(std::string_view name) const {
    auto hash = hash_djb2((unsigned char *) name.data());
    return m_properties.find(hash) != m_properties.end();
}

bool Widget::removeProperty(std::string_view name) {
    auto hash = hash_djb2((unsigned char *) name.data());

    if (auto it = m_properties.find(hash); it != m_properties.end()) {
        m_properties.erase(it);
        return true;
    }

    return false;
}

void Widget::setEventListener(Event::Id event, const EventListener &listener) {
    m_eventListeners[event] = listener;
}

void Widget::setEventAnimation(Event::Id event, Widgets::AnimationPtr animation) {
    animation->setWidget(this);
    m_eventAnimations[event] = std::move(animation);
}

bool Widget::hasAnimation(Event::Id event) const {
    return m_eventAnimations.find(event) != m_eventAnimations.end();
}

const Widgets::AnimationPtr& Widget::getAnimation(Event::Id event) const {
    return m_eventAnimations.at(event);
}

void Widget::addAnimation(Widgets::AnimationPtr animation) {
    animation->setWidget(this);
    if (!animation->isActive())
        animation->start();
    m_animations.emplace_front(std::move(animation));
}

const Widget::Animations& Widget::getAnimations() const {
    return m_animations;
}

void Widget::event(const Event &event) {
    auto id = event.getId();

    if (auto it = m_eventAnimations.find(id); it != m_eventAnimations.end())
        it->second->start();

    if (auto it = m_eventListeners.find(id); it != m_eventListeners.end()) {
        it->second(this, event);
    }
}

void Widget::addScript(const std::string &path, Lua *lua) {
    addLuaScript(path, true, lua);
}

void Widget::addScriptSource(const std::string &source, Lua *lua) {
    addLuaScript(source, false, lua);
}

void Widget::addLuaScript(const std::string &s, bool path, Lua *lua) {
    lua = lua ? lua : &Engine::getLua();

    Lua::Locker locker(lua);

    if (m_luaEnv.lua_state() != lua->state()->lua_state()) {
        m_luaEnv.abandon();
        m_luaEnv = lua->createEnvironment(lua->getGlobalEnvironment());
    }

    auto &state = *lua->state();

    auto printErr = [&](sol::error &error) {
        if (!path) {
            Log::error() << error.what();
        } else {
            Log::error() << "In " << s << ":\n" << error.what();
        }
    };

    auto result = path ?
        state.script(lua->readStr(s), m_luaEnv) :
        state.script(s, m_luaEnv);

    if (!result.valid()) {
        sol::error error = result;
        printErr(error);
    }
}

template<typename T>
inline uint asBitFlag(T flag) {
    uint fl = static_cast<uint>(flag);
    return 1u << fl;
}

void Widget::setFlag(Flag flag, bool on) {
    uint fl = asBitFlag(flag);
    m_flags = on ? (m_flags | fl) : (m_flags & ~fl);
}

bool Widget::isFlagEnabled(Flag flag) const {
    uint fl = asBitFlag(flag);
    return m_flags & fl;
}

int Widget::matchParentWidth(Widget *child) {
    return getContentWidth();
}

int Widget::matchParentHeight(Widget *child) {
    return getContentHeight();
}

void Widget::setMeasuredDimension(int width, int height) {
    setGeometry({
        getX(),
        getY(),
        width + getPaddingLeft() + getPaddingRight(),
        height + getPaddingTop() + getPaddingBottom()
    });
}

void Widget::onAnimate() {}

void Widget::onMeasure(int &width, int &height) {
    auto contentWidth = [&](int width) {
        return width - getPaddingLeft() - getPaddingRight();
    };

    auto contentHeight = [&](int height) {
        return height - getPaddingTop() - getPaddingBottom();
    };

    Widget* thisParent = m_parent;

    switch (m_horizontalPolicy) {
        case SizePolicy::Minimum: width = contentWidth(getMinWidth()); break;
        case SizePolicy::Maximum: width = contentWidth(getMaxWidth()); break;
        case SizePolicy::MatchParent:
            if (thisParent) {
                width = contentWidth(thisParent->matchParentWidth(this));
            }
            break;
        default: break;
    }

    switch (m_verticalPolicy) {
        case SizePolicy::Minimum: height = contentHeight(getMinHeight()); break;
        case SizePolicy::Maximum: height = contentHeight(getMaxHeight()); break;
        case SizePolicy::MatchParent:
            if (thisParent) {
                height = contentHeight(thisParent->matchParentHeight(this));
            }
            break;
        default: break;
    }
}

void Widget::onLayout() {}

void Widget::draw(Painter &painter) {
    print_call("draw(): " + getName());

    m_framebuffer->bind();

    if (isFlagEnabled(Flag::SizeChanged)) {
        setFlag(Flag::SizeChanged, false);
        m_framebuffer->resizeAttachments(getWidth(), getHeight());
    }

    m_framebuffer->setClearColor(0, 0, 0, 0);
    m_framebuffer->clearColorBuffer();

    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // TODO: move to Engine

    Engine::setViewport(getWidth(), getHeight());
    painter.setViewport(getWidth(), getHeight());

    painter.setTransform(glm::mat4(1.0f));
    painter.setOpacity(1.0f);

    if (m_background.getSource() != Paint::Source::None) {
        onDrawBackground(painter);
    }

    Engine::setViewport(m_paddingLeft, m_paddingBottom, getContentWidth(), getContentHeight());
    painter.setViewport(getContentWidth(), getContentHeight());

    painter.setPaint({});
}

void Widget::onDrawBackground(Painter &painter) {
    if (m_luaEnv.valid() && m_luaEnv["onDrawBackground"].valid()) {
        auto result = m_luaEnv["onDrawBackground"].get<sol::function>()(this, painter);

        if (!result.valid()) {
            sol::error error = result;
            Log::error() << error.what();
        }
    } else {
        painter.setPaint(m_background);
        painter.drawRect(0, 0, getWidth(), getHeight());
    }
}

void Widget::onGeometryChanged(const RectI &geometry) {}

Widget::Filtering Widget::parseFiltering(std::string_view str) {
    if (str == "nearest") {
        return Filtering::Nearest;
    } else if (str == "linear") {
        return Filtering::Linear;
    } else {
        Log::error("Widget") << "Unknown filtering method '" << str.data() << "'";
        return Filtering::Nearest;
    }
}

std::string Widget::getString(const char *str) {
    return Resources::instance()->parse(str).as<std::string>();
}

float Widget::getDimenPxF(const char *str) {
    bool error;
    auto result = Resources::instance()->parse(str, &error);

    if (error) { // try to parse using Units, not Resources
        return Units::parse(str);
    }

    return result.as<Dimen>().pixels();
}

int Widget::getDimenPx(const char *str) {
    return static_cast<int>(getDimenPxF(str));
}

Color Widget::getColor(const char *str) {
    bool error;
    auto result = Resources::instance()->parse(str, &error);

    if (error) { // try to parse using Color, not Resources
        return Color::parseColor(str);
    }

    return result.as<Color>();
}

// Note: you can use resources (e.g. @string/app_name) for all Widget attributes, except the following:
// visible, name, horizontalSizePolicy, verticalSizePolicy, filtering
// these attributes must be specified explicitly (further - explicit attributes)

void Widget::fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) {
    for (pugi::xml_attribute attr : node.attributes()) {
        auto isAttr = [&](const char *name) {
            return strcmp(attr.name(), name) == 0;
        };

        auto parseSizePolicy = [&]() {
            std::string_view value = attr.as_string();

            if (value == "fixed") {
                return SizePolicy::Fixed;
            } else if (value == "minimum") {
                return SizePolicy::Minimum;
            } else if (value == "maximum") {
                return SizePolicy::Maximum;
            } else if (value == "preferred") {
                return SizePolicy::Preferred;
            } else if (value == "match_parent") {
                return SizePolicy::MatchParent;
            }

            Log::error("Widget") << "Unknown size policy '" << value.data() << "'";

            return SizePolicy::Fixed;
        };

        auto metrics_parse = [&]() { return getDimenPx(attr.as_string()); };

        auto getFloat = [&]() {
            bool error;
            auto result = Resources::instance()->parse(attr.as_string(), &error);

            if (error) { // try to parse using pugixml
                return attr.as_float();
            }

            return result.as<float>();
        };

        auto getString = [&]() { return Widget::getString(attr.as_string()); };

        if (isAttr("visible")) {
            setFlag(Flag::Visible, attr.as_bool());
        } else if (isAttr("x")) {
            setX(metrics_parse());
        } else if (isAttr("y")) {
            setY(metrics_parse());
        } else if (isAttr("width")) {
            setWidth(metrics_parse());
        } else if (isAttr("height")) {
            setHeight(metrics_parse());
        } else if (isAttr("minWidth")) {
            setMinWidth(metrics_parse());
        } else if (isAttr("minHeight")) {
            setMinHeight(metrics_parse());
        } else if (isAttr("maxWidth")) {
            setMaxWidth(metrics_parse());
        } else if (isAttr("maxHeight")) {
            setMaxHeight(metrics_parse());
        } else if (isAttr("name")) {
            setName(attr.as_string());
        } else if (isAttr("background")) {
            m_background.setTexture(TexturePathLoader::load(getString(), io).texture);
        } else if (isAttr("backgroundColor")) {
            m_background.setColor(getColor(attr.as_string()));
        } else if (isAttr("padding")) {
            auto padding = tulz::StringUtils::split(getString(), " ");

            auto left = Units::parse<int>(padding[0].c_str());
            auto top = Units::parse<int>(padding[1].c_str());
            auto right = Units::parse<int>(padding[2].c_str());
            auto bottom = Units::parse<int>(padding[3].c_str());

            setPadding(left, top, right, bottom);
        } else if (isAttr("paddingLeft")) {
            setPaddingLeft(metrics_parse());
        } else if (isAttr("paddingTop")) {
            setPaddingTop(metrics_parse());
        } else if (isAttr("paddingRight")) {
            setPaddingRight(metrics_parse());
        } else if (isAttr("paddingBottom")) {
            setPaddingBottom(metrics_parse());
        } else if (isAttr("rotate")) {
            setRotate(getFloat());
        } else if (isAttr("scaleX")) {
            setScaleX(getFloat());
        } else if (isAttr("scaleY")) {
            setScaleY(getFloat());
        } else if (isAttr("opacity")) {
            setOpacity(getFloat());
        } else if (isAttr("horizontalSizePolicy")) {
            setHorizontalSizePolicy(parseSizePolicy());
        } else if (isAttr("verticalSizePolicy")) {
            setVerticalSizePolicy(parseSizePolicy());
        } else if (isAttr("filtering")) {
            setFiltering(parseFiltering(attr.as_string()));
        } else if (isAttr("script")) {
            addScript(getString());
        } else if (auto name = attr.name(); strstr(name, "p_") == name) { // property
            name += strlen("p_");

            // parse value
            auto value = attr.value();

            char* end;

            if (auto int_value = (int) strtol(value, &end, 0); *end == '\0') {
                setProperty(name, int_value);
            } else if (float float_value = strtof(value, &end); *end == '\0') {
                setProperty(name, float_value);
            } else {
                bool error;

                if (auto res_value = Resources::instance()->parse(value, &error); !error) {
                    if (res_value.is<std::string>()) {
                        setProperty(name, res_value.as<std::string>());
                    } else if (res_value.is<int>()) {
                        setProperty(name, res_value.as<int>());
                    } else if (res_value.is<float>()) {
                        setProperty(name, res_value.as<float>());
                    } else if (res_value.is<Dimen>()) {
                        setProperty(name, res_value.as<Dimen>().pixels());
                    } else if (res_value.is<Color>()) {
                        setProperty(name, static_cast<int>(res_value.as<Color>().value()));
                    }
                } else if (auto unit_value = Units::try_parse(value, &error); !error) {
                    setProperty(name, unit_value);
                } else if (strcmp(value, "true") == 0) {
                    setProperty(name, true);
                } else if (strcmp(value, "false") == 0) {
                    setProperty(name, false);
                } else {
                    setProperty(name, std::string(value));
                }
            }
        }
    }
}

inline auto parseXML(const std::string &xml) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xml.c_str());

    if (!result) {
        Log::error("Widget XML Parser", result.description());
    }

    return doc;
}

bool Widget::fromXML(const std::string &xml, const std::shared_ptr<IOSystem> &io) {
    if (auto doc = parseXML(xml); !doc.document_element().empty()) {
        fromXML(doc.document_element(), io);
        return true;
    }

    return false;
}

bool Widget::fromXMLFile(const std::string &file, const std::shared_ptr<IOSystem> &io) {
    auto stream = io->open(file, IOStream::Mode::ReadText);
    auto xml = stream->readStr();
    stream->close();
    return fromXML(xml, io);
}

bool Widget::fromXML(const std::string &xml) {
    return fromXML(xml, Engine::getDefaultIOSystem());
}

bool Widget::fromXMLFile(const std::string &file) {
    return fromXMLFile(file, Engine::getDefaultIOSystem());
}

WidgetPtr Widget::constructFromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) {
    WidgetPtr rootWidget(TypeRegistry::create<Widget>(node.name()));
    rootWidget->fromXML(node, io);
    return rootWidget;
}

WidgetPtr Widget::constructFromXML(const std::string &xml, const std::shared_ptr<IOSystem> &io) {
    if (auto doc = parseXML(xml); !doc.document_element().empty()) {
        return constructFromXML(doc.document_element(), io);
    }

    return nullptr;
}

WidgetPtr Widget::constructFromXMLFile(const std::string &file, const std::shared_ptr<IOSystem> &io) {
    auto stream = io->open(file, IOStream::Mode::ReadText);
    auto xml = stream->readStr();
    stream->close();
    return constructFromXML(xml, io);
}

WidgetPtr Widget::constructFromXML(const std::string &xml) {
    return constructFromXML(xml, Engine::getDefaultIOSystem());
}

WidgetPtr Widget::constructFromXMLFile(const std::string &file) {
    return constructFromXMLFile(file, Engine::getDefaultIOSystem());
}
}
