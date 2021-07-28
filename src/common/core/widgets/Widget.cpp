#include <algine/core/widgets/Widget.h>

#include <algine/core/shader/ShaderCreator.h>
#include <algine/core/painter/Painter.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/Engine.h>

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
      m_scaleY(1.0f)
{
    m_texture = PtrMaker::make();
    m_texture->bind();
    m_texture->setFormat(Texture::RGBA32F);
    m_texture->setDimensions(getWidth(), getHeight());
    m_texture->setParams(Texture2D::defaultParams());
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
        requireParentRedraw();

        if (geometry.getWidth() != m_geometry.getWidth() || geometry.getHeight() != m_geometry.getHeight()) {
            setFlag(Flag::SizeChanged);
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
        setFlag(Flag::RedrawRequired, false);
        measure();
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

void Widget::measure() {}

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
        }
    }
}

bool Widget::fromXML(const std::string &xml, const std::shared_ptr<IOSystem> &io) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xml.c_str());

    if (!result) {
        fprintf(stderr, "Widget: XML parsing error: %s\n", result.description());
        return false;
    }

    fromXML(doc.document_element(), io);

    return true;
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
}
