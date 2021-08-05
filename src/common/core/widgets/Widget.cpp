#include <algine/core/widgets/Widget.h>

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

#define requireParentRedraw() if (m_parent) m_parent->setFlag(Flag::RedrawRequired)

namespace algine {

Widget::Widget()
    : m_flags(static_cast<Flags>(Flag::Visible)),
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
        {Texture::MagFilter, Texture::Nearest}
    });
    m_texture->update();

    m_framebuffer = PtrMaker::make();
    m_framebuffer->bind();
    m_framebuffer->attachTexture(m_texture, Framebuffer::ColorAttachmentZero);

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
        }

        if (geometry.getX() != getX() || geometry.getY() != getY()) {
            requireParentRedraw();
        }

        geometryChanged(geometry);

        m_geometry = geometry;
    }
}

const RectI& Widget::getGeometry() const {
    return m_geometry;
}

void Widget::setFlag(Flag flag, bool on) {
    if (flag == Flag::SizeChanged && on) {
        setFlag(Flag::RedrawRequired);
    }

    if (flag == Flag::Visible || (flag == Flag::RedrawRequired && on)) {
        requireParentRedraw();
    }

    uint fl = static_cast<uint>(flag);
    m_flags = on ? (m_flags | fl) : (m_flags & ~fl);
}

bool Widget::isFlagEnabled(Flag flag) const {
    uint fl = static_cast<uint>(flag);
    return m_flags & fl;
}

void Widget::setFlags(Flags flags) {
    m_flags = flags;
}

Widget::Flags Widget::getFlags() const {
    return m_flags;
}

void Widget::setVisible(bool visible) {
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
    m_background = paint;
}

const Paint& Widget::getBackground() const {
    return m_background;
}

void Widget::setPadding(int left, int top, int right, int bottom) {
    m_paddingLeft = left;
    m_paddingTop = top;
    m_paddingRight = right;
    m_paddingBottom = bottom;
}

void Widget::setPaddingLeft(int padding) {
    m_paddingLeft = padding;
}

void Widget::setPaddingTop(int padding) {
    m_paddingTop = padding;
}

void Widget::setPaddingRight(int padding) {
    m_paddingRight = padding;
}

void Widget::setPaddingBottom(int padding) {
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
    setFlag(Flag::RedrawRequired);
    m_rotate = degrees;
}

float Widget::getRotate() const {
    return m_rotate;
}

void Widget::setScaleX(float scale) {
    requireParentRedraw();
    m_scaleX = scale;
}

void Widget::setScaleY(float scale) {
    requireParentRedraw();
    m_scaleY = scale;
}

void Widget::setScale(float scale) {
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
    setFlag(Flag::RedrawRequired);
    m_horizontalPolicy = horizontal;
    m_verticalPolicy = vertical;
}

void Widget::setHorizontalSizePolicy(SizePolicy policy) {
    setFlag(Flag::RedrawRequired);
    m_horizontalPolicy = policy;
}

void Widget::setVerticalSizePolicy(SizePolicy policy) {
    setFlag(Flag::RedrawRequired);
    m_verticalPolicy = policy;
}

SizePolicy Widget::getHorizontalSizePolicy() const {
    return m_horizontalPolicy;
}

SizePolicy Widget::getVerticalSizePolicy() const {
    return m_verticalPolicy;
}

void Widget::setFiltering(Filtering filtering) {
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

void Widget::invalidate() {
    setFlag(Flag::RedrawRequired);
    setFlag(Flag::SizeChanged);
}

void Widget::display(const DisplayOptions &options) {
    if (!isVisible()) {
        return;
    }

    auto painter = options.painter;

    if (isFlagEnabled(Flag::RedrawRequired)) {
        int width = getContentWidth();
        int height = getContentHeight();

        // calculate content width/height
        measure(width, height);

        setGeometry({
            getX(),
            getY(),
            width + getPaddingLeft() + getPaddingRight(),
            height + getPaddingTop() + getPaddingBottom()
        });

        setFlag(Flag::RedrawRequired, false);
        drawingStart(*painter);
        draw(*painter);
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

void Widget::measure(int &width, int &height) {
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
                width = contentWidth(m_parent->getContentWidth());
            }
            break;
        default: break;
    }

    switch (m_verticalPolicy) {
        case SizePolicy::Minimum: height = contentHeight(getMinHeight()); break;
        case SizePolicy::Maximum: height = contentHeight(getMaxHeight()); break;
        case SizePolicy::MatchParent:
            if (m_parent) {
                height = contentHeight(m_parent->getContentHeight());
            }
            break;
        default: break;
    }
}

void Widget::drawingStart(Painter &painter) {
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
        drawBackground(painter);
    }

    Engine::setViewport(m_paddingLeft, m_paddingBottom, getContentWidth(), getContentHeight());
    painter.setViewport(getContentWidth(), getContentHeight());

    painter.setPaint({});
}

void Widget::drawBackground(Painter &painter) {
    painter.setPaint(m_background);
    painter.drawRect(0, 0, getWidth(), getHeight());
}

void Widget::geometryChanged(const RectI &geometry) {}

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

        if (isAttr("visible")) {
            setFlag(Flag::Visible, attr.as_bool());
        } else if (isAttr("x")) {
            setX(attr.as_int());
        } else if (isAttr("y")) {
            setY(attr.as_int());
        } else if (isAttr("width")) {
            setWidth(attr.as_int());
        } else if (isAttr("height")) {
            setHeight(attr.as_int());
        } else if (isAttr("minWidth")) {
            setMinWidth(attr.as_int());
        } else if (isAttr("minHeight")) {
            setMinHeight(attr.as_int());
        } else if (isAttr("maxWidth")) {
            setMaxWidth(attr.as_int());
        } else if (isAttr("maxHeight")) {
            setMaxHeight(attr.as_int());
        } else if (isAttr("name")) {
            setName(attr.as_string());
        } else if (isAttr("background")) {
            TextureFileInfo info;
            info.flip = true;
            info.path = attr.as_string();
            info.ioSystem = io;

            Texture2DPtr background = PtrMaker::make();
            background->bind();
            background->fromFile(info);

            m_background.setTexture(background);
        } else if (isAttr("backgroundColor")) {
            m_background.setColor(Color::parseColor(attr.as_string()));
        } else if (isAttr("padding")) {
            auto padding = tulz::StringUtils::split(attr.as_string(), " ");

            auto left = std::stoi(padding[0]);
            auto top = std::stoi(padding[1]);
            auto right = std::stoi(padding[2]);
            auto bottom = std::stoi(padding[3]);

            setPadding(left, top, right, bottom);
        } else if (isAttr("paddingLeft")) {
            setPaddingLeft(attr.as_int());
        } else if (isAttr("paddingTop")) {
            setPaddingTop(attr.as_int());
        } else if (isAttr("paddingRight")) {
            setPaddingRight(attr.as_int());
        } else if (isAttr("paddingBottom")) {
            setPaddingBottom(attr.as_int());
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
            if (auto filtering = attr.as_string(); strcmp(filtering, "nearest") == 0) {
                setFiltering(Filtering::Nearest);
            } else if (strcmp(filtering, "linear") == 0) {
                setFiltering(Filtering::Linear);
            } else {
                Log::error("Widget") << "Unknown filtering method '" << filtering << "'" << Log::end;
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
