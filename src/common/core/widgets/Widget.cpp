#include <algine/core/widgets/Widget.h>
#include <algine/core/widgets/Units.h>
#include <algine/core/shader/ShaderCreator.h>
#include <algine/core/painter/Painter.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/Engine.h>
#include <algine/core/log/Log.h>
#include <algine/core/TypeRegistry.h>

#include <tulz/StringUtils.h>
#include <glm/ext/matrix_transform.hpp>

#include <pugixml.hpp>

#include <cstring>

#include "core/djb2.h"
#include "TexturePathLoader.h"

#define requireParentRedraw() if (m_parent) m_parent->invalidate()
#define requireParentLayout() if (m_parent) m_parent->requestLayout()

#define continue_if(condition) if (!(condition)) return

//#define ALGINE_WIDGET_TRACE_CALLS

#ifdef ALGINE_WIDGET_TRACE_CALLS
#define print_call(str) Log::info("Widget") << (str) << Log::end
#else
#define print_call(str)
#endif

namespace algine {
Widget::Widget()
    : m_flags(),
      m_geometry(0, 0, 128, 128),
      m_parent(nullptr),
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
      m_horizontalPolicy(SizePolicy::Fixed),
      m_verticalPolicy(SizePolicy::Fixed)
{
    m_texture = PtrMaker::make();
    m_texture->bind();
    m_texture->setFormat(Texture::RGBA16F);
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
    setGeometry({m_geometry.getX(), m_geometry.getY(), width, m_geometry.getHeight()});
}

void Widget::setHeight(int height) {
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

void Widget::setParent(Widget *parent) {
    m_parent = parent;
}

Widget* Widget::getParent() const {
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

void Widget::display(const DisplayOptions &options) {
    if (!isVisible()) {
        return;
    }

    auto painter = options.painter;

    measure();
    layout();

    if (isFlagEnabled(Flag::RedrawRequired)) {
        setFlag(Flag::RedrawRequired, false);

        draw(*painter);
        onDraw(*painter);
    }

    if (options.parentFramebuffer) {
        options.parentFramebuffer->bind();
    } else {
        Engine::defaultFramebuffer()->bind();
    }

    int parentContentWidth = options.parentWidth - options.parentPaddingLeft - options.parentPaddingRight;
    int parentContentHeight = options.parentHeight - options.parentPaddingTop - options.parentPaddingBottom;

    Engine::setViewport(options.parentPaddingLeft, options.parentPaddingBottom,
                        parentContentWidth, parentContentHeight);
    painter->setViewport(parentContentWidth, parentContentHeight);

    painter->setTransform(glm::mat4(1.0f));

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // TODO: move to Engine

    auto width = static_cast<float>(getWidth());
    auto height = static_cast<float>(getHeight());
    auto x = static_cast<float>(getX());
    auto y = static_cast<float>(getY());

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

PointI Widget::toLocalPoint(const PointI &globalPoint) const {
    PointI localPoint {
        globalPoint.getX() - getX(),
        globalPoint.getY() - getY()
    };

    if (m_parent) {
        localPoint -= {m_parent->getPaddingLeft(), m_parent->getPaddingTop()};
    }

    if (m_rotate != 0.0f || m_scaleX != 1.0f || m_scaleY != 1.0f) {
        auto width = getWidth();
        auto height = getHeight();

        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), {m_scaleX, m_scaleY, 1.0f});
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotate), {0.0f, 0.0f, 1.0f});

        glm::vec4 lp_centered {localPoint.getX() - width / 2, localPoint.getY() - height / 2, 0.0f, 1.0f};
        glm::vec4 lp = glm::inverse(rotation * scaling) * lp_centered + glm::vec4(width / 2, height / 2, 0.0f, 0.0f);

        return {(int) lp.x, (int) lp.y};
    }

    return localPoint;
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

void Widget::setProperty(const char *name, const Property &property) {
    auto hash = hash_djb2((unsigned char *) name);
    m_properties[hash] = property;
}

const Widget::Property& Widget::getProperty(const char *name) const {
    auto hash = hash_djb2((unsigned char *) name);
    return m_properties.at(hash);
}

Widget::Property& Widget::property(const char *name) {
    auto hash = hash_djb2((unsigned char *) name);
    return m_properties[hash];
}

bool Widget::hasProperty(const char *name) const {
    auto hash = hash_djb2((unsigned char *) name);
    return m_properties.find(hash) != m_properties.end();
}

bool Widget::removeProperty(const char *name) {
    auto hash = hash_djb2((unsigned char *) name);

    if (auto it = m_properties.find(hash); it != m_properties.end()) {
        m_properties.erase(it);
        return true;
    }

    return false;
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

void Widget::onMeasure(int &width, int &height) {
    auto contentWidth = [&](int width) {
        return width - getPaddingLeft() - getPaddingRight();
    };

    auto contentHeight = [&](int height) {
        return height - getPaddingTop() - getPaddingBottom();
    };

    switch (m_horizontalPolicy) {
        case SizePolicy::Minimum: width = contentWidth(getMinWidth()); break;
        case SizePolicy::Maximum: width = contentWidth(getMaxWidth()); break;
        case SizePolicy::MatchParent:
            if (m_parent) {
                width = contentWidth(m_parent->matchParentWidth(this));
            }
            break;
        default: break;
    }

    switch (m_verticalPolicy) {
        case SizePolicy::Minimum: height = contentHeight(getMinHeight()); break;
        case SizePolicy::Maximum: height = contentHeight(getMaxHeight()); break;
        case SizePolicy::MatchParent:
            if (m_parent) {
                height = contentHeight(m_parent->matchParentHeight(this));
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

    if (m_background.getSource() != Paint::Source::None) {
        onDrawBackground(painter);
    }

    Engine::setViewport(m_paddingLeft, m_paddingBottom, getContentWidth(), getContentHeight());
    painter.setViewport(getContentWidth(), getContentHeight());

    painter.setPaint({});
}

void Widget::onDrawBackground(Painter &painter) {
    painter.setPaint(m_background);
    painter.drawRect(0, 0, getWidth(), getHeight());
}

void Widget::onGeometryChanged(const RectI &geometry) {}

Widget::Filtering Widget::parseFiltering(const char *str) {
    if (strcmp(str, "nearest") == 0) {
        return Filtering::Nearest;
    } else if (strcmp(str, "linear") == 0) {
        return Filtering::Linear;
    } else {
        Log::error("Widget") << "Unknown filtering method '" << str << "'" << Log::end;
        return Filtering::Nearest;
    }
}

void Widget::fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) {
    for (pugi::xml_attribute attr : node.attributes()) {
        auto isAttr = [&](const char *name) {
            return strcmp(attr.name(), name) == 0;
        };

        auto parseSizePolicy = [&]() {
            auto value = attr.value();

            if (strcmp(value, "fixed") == 0) {
                return SizePolicy::Fixed;
            } else if (strcmp(value, "minimum") == 0) {
                return SizePolicy::Minimum;
            } else if (strcmp(value, "maximum") == 0) {
                return SizePolicy::Maximum;
            } else if (strcmp(value, "preferred") == 0) {
                return SizePolicy::Preferred;
            } else if (strcmp(value, "match_parent") == 0) {
                return SizePolicy::MatchParent;
            }

            Log::error("Widget") << "Unknown size policy '" << value << "'" << Log::end;

            return SizePolicy::Fixed;
        };

        auto metrics_parse = [&]() {
            return Units::parse<int>(attr.as_string());
        };

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
            m_background.setTexture(TexturePathLoader::load(attr.as_string(), io).texture);
        } else if (isAttr("backgroundColor")) {
            m_background.setColor(Color::parseColor(attr.as_string()));
        } else if (isAttr("padding")) {
            auto padding = tulz::StringUtils::split(attr.as_string(), " ");

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
            setRotate(attr.as_float());
        } else if (isAttr("scaleX")) {
            setScaleX(attr.as_float());
        } else if (isAttr("scaleY")) {
            setScaleY(attr.as_float());
        } else if (isAttr("horizontalSizePolicy")) {
            setHorizontalSizePolicy(parseSizePolicy());
        } else if (isAttr("verticalSizePolicy")) {
            setVerticalSizePolicy(parseSizePolicy());
        } else if (isAttr("filtering")) {
            setFiltering(parseFiltering(attr.as_string()));
        } else if (auto name = attr.name(); strstr(name, "p_") == name) { // property
            name += strlen("p_");

            auto value = attr.value();

            char* end;

            if (auto int_value = (int) strtol(value, &end, 0); *end == '\0') {
                setProperty(name, int_value);
            } else if (float float_value = strtof(value, &end); *end == '\0') {
                setProperty(name, float_value);
            } else {
                bool unitError;

                if (auto unit_value = Units::try_parse(value, &unitError); !unitError) {
                    setProperty(name, unit_value);
                } else if (strcmp(value, "true") == 0) {
                    setProperty(name, true);
                } else if (strcmp(value, "false") == 0) {
                    setProperty(name, false);
                } else { // std::string
                    setProperty(name, value);
                }
            }
        }
    }
}

inline auto parseXML(const std::string &xml) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xml.c_str());

    if (!result) {
        Log::error("Widget XML Parser") << result.description() << Log::end;
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
