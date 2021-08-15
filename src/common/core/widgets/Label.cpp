#include <algine/core/widgets/Label.h>
#include <algine/core/widgets/Alignment.h>
#include <algine/core/painter/Painter.h>

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
    requestLayout();
    invalidate();
    m_text = text;
}

const std::string& Label::getText() const {
    return m_text;
}

void Label::setFont(const Font &font) {
    requestLayout();
    invalidate();
    m_fontMetrics.setFont(font);
}

void Label::setFont(const Font &font, uint size) {
    requestLayout();
    invalidate();
    m_fontMetrics.setFont(font, size);
}

void Label::setFontSize(uint size) {
    requestLayout();
    invalidate();
    m_fontMetrics.setFontSize(size);
}

void Label::setFontColor(const Color &color) {
    invalidate();
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
    invalidate();
    m_textAlignment = alignment;
}

uint Label::getTextAlignment() const {
    return m_textAlignment;
}

void Label::measure(int &width, int &height) {
    Widget::measure(width, height);

    auto preferredWidth = [](const RectI &rect) {
        return rect.getWidth() - rect.getX();
    };

    auto preferredHeight = [](const RectI &rect) {
        return rect.getHeight();
    };

    if (m_horizontalPolicy == SizePolicy::Preferred && m_verticalPolicy == SizePolicy::Preferred) {
        auto rect = m_fontMetrics.boundingRect(m_text);
        width = preferredWidth(rect);
        height = preferredHeight(rect);
    }

    switch (m_horizontalPolicy) {
        case SizePolicy::Preferred:
            width = preferredWidth(m_fontMetrics.boundingRect(m_text));
            break;
        default: break;
    }

    switch (m_verticalPolicy) {
        case SizePolicy::Preferred:
            height = preferredHeight(m_fontMetrics.boundingRect(m_text));
            break;
        default: break;
    }
}

void Label::onDraw(Painter &painter) {
    float x;
    float y;
    auto boundingRect = m_fontMetrics.boundingRect(m_text);

    auto br_x = boundingRect.getX();
    auto br_y = boundingRect.getY();
    auto br_width = boundingRect.getWidth();
    auto br_height = boundingRect.getHeight();

    uint alignmentX = m_textAlignment & 0b0011u;
    uint alignmentY = m_textAlignment & 0b1100u;

    switch (alignmentX) {
        case Alignment::Right:
            x = (float) (getContentWidth() - br_width - br_x);
            break;
        case Alignment::Left:
            x = (float) -br_x;
            break;
        default:
            x = (float) (getContentWidth() - (br_width + br_x)) / 2.0f;
            break;
    }

    switch (alignmentY) {
        case Alignment::Top:
            y = (float) -br_y;
            break;
        case Alignment::Bottom:
            y = (float) (getContentHeight() - br_height - br_y);
            break;
        default:
            y = (float) (getContentHeight() + br_height) / 2.0f - (float) (br_height + br_y);
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
            setTextAlignment(Alignment::parse(attr.as_string()));
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
