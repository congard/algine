#include <algine/core/widgets/Label.h>
#include <algine/core/painter/Painter.h>

#include <tulz/StringUtils.h>

#include <pugixml.hpp>

#include <cstring>

namespace algine {
Label::Label()
    : m_textAlignment(Alignment::Center),
      m_fontColor(0xff000000)
{
    m_fontMetrics.setFontSize(24);
}

Label::Label(const std::string &text): Label() {
    setText(text);
}

void Label::setText(const std::string &text) {
    setFlag(Widget::Flag::RedrawRequired);
    m_text = text;
}

const std::string& Label::getText() const {
    return m_text;
}

void Label::setFont(const Font &font) {
    setFlag(Widget::Flag::RedrawRequired);
    m_fontMetrics.setFont(font);
}

void Label::setFont(const Font &font, uint size) {
    setFlag(Widget::Flag::RedrawRequired);
    m_fontMetrics.setFont(font, size);
}

void Label::setFontSize(uint size) {
    setFlag(Widget::Flag::RedrawRequired);
    m_fontMetrics.setFontSize(size);
}

void Label::setFontColor(const Color &color) {
    setFlag(Widget::Flag::RedrawRequired);
    m_fontColor = color;
}

const Font& Label::getFont() const {
    return m_fontMetrics.getFont();
}

uint Label::getFontSize() const {
    return m_fontMetrics.getFontSize();
}

const Color& Label::getFontColor() const {
    return m_fontColor;
}

void Label::setTextAlignment(uint alignment) {
    setFlag(Widget::Flag::RedrawRequired);
    m_textAlignment = alignment;
}

uint Label::getTextAlignment() const {
    return m_textAlignment;
}

// TODO: it seems that measure() should take RectI as an argument: currently required size

void Label::measure() {
    Widget::measure();
    // TODO: implement, add SizePolicy
}

void Label::draw(Painter &painter) {
    float x;
    float y;
    auto boundingRect = m_fontMetrics.boundingRect(m_text);

    uint alignmentX = m_textAlignment & 0b0011u;
    uint alignmentY = m_textAlignment & 0b1100u;

    switch (alignmentX) {
        case Alignment::Right:
            x = (float) (getContentWidth() - boundingRect.getWidth() - boundingRect.getX());
            break;
        case Alignment::Left:
            x = (float) -boundingRect.getX();
            break;
        default:
            x = (float) (getContentWidth() - (boundingRect.getWidth() + boundingRect.getX())) / 2.0f;
            break;
    }

    switch (alignmentY) {
        case Alignment::Top:
            y = (float) boundingRect.getHeight();
            break;
        case Alignment::Bottom:
            y = (float) getContentHeight();
            break;
        default:
            y = (float) (getContentHeight() + boundingRect.getHeight()) / 2.0f;
            break;
    }

    painter.setFont(getFont(), getFontSize());
    painter.setPaint(Paint(m_fontColor));
    painter.drawText(m_text, x, y);
}

void Label::fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) {
    Widget::fromXML(node, io);

    const pugi::char_t *fontName = nullptr;
    const pugi::char_t *fontStyle = nullptr;

    for (pugi::xml_attribute attr : node.attributes()) {
        auto isAttr = [&](const char *name) {
            return strcmp(attr.name(), name) == 0;
        };

        if (isAttr("text")) {
            setText(attr.as_string());
        } else if (isAttr("font")) {
            fontName = attr.as_string();
        } else if (isAttr("fontStyle")) {
            fontStyle = attr.as_string();
        } else if (isAttr("fontSrc")) {
            setFont(Font(attr.as_string()));
        } else if (isAttr("fontSize")) {
            setFontSize(attr.as_uint());
        } else if (isAttr("fontColor")) {
            setFontColor(Color::parseColor(attr.as_string()));
        } else if (isAttr("textAlignment")) {
            auto alignmentValues = tulz::StringUtils::split(attr.as_string(), "|");
            uint alignment = Alignment::Center;

            for (std::string &value : alignmentValues) {
                if (value == "center") {
                    alignment |= Alignment::Center;
                } else if (value == "left") {
                    alignment |= Alignment::Left;
                } else if (value == "right") {
                    alignment |= Alignment::Right;
                } else if (value == "top") {
                    alignment |= Alignment::Top;
                } else if (value == "bottom") {
                    alignment |= Alignment::Bottom;
                }
            }

            setTextAlignment(alignment);
        }
    }

    if (fontName) {
        Font::Style style = Font::Style::Any;

        if (fontStyle) {
            if (strcmp(fontStyle, "regular") == 0) {
                style = Font::Style::Regular;
            } else if (strcmp(fontStyle, "bold") == 0) {
                style = Font::Style::Bold;
            } else if (strcmp(fontStyle, "italic") == 0) {
                style = Font::Style::Italic;
            }
        }

        setFont(Font(fontName, style));
    }
}
}
