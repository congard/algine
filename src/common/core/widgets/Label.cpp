#include <algine/core/widgets/Label.h>
#include <algine/core/widgets/Alignment.h>
#include <algine/core/widgets/Units.h>
#include <algine/core/painter/Painter.h>

#include <pugixml.hpp>

#include <cstring>

namespace algine {
Label::Label()
    : m_textAlignment(Alignment::Center),
      m_fontColor(0xff000000)
{
    m_fontMetrics.setFontSize(24_dp);
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

void Label::onMeasure(int &width, int &height) {
    Widget::onMeasure(width, height);

    auto preferredWidth = [](const RectI &rect) {
        return rect.getWidth();
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
            x = (float) -br_x + (float) (getContentWidth() - br_width) / 2.0f;
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

// Explicit attributes: fontStyle, textAlignment

void Label::fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) {
    Widget::fromXML(node, io);

    std::string fontName;
    std::string fontStyle;

    for (pugi::xml_attribute attr : node.attributes()) {
        auto isAttr = [&](const char *name) {
            return strcmp(attr.name(), name) == 0;
        };

        auto attr_str = attr.as_string();

        if (isAttr("text")) {
            setText(getString(attr_str));
        } else if (isAttr("font")) {
            fontName = getString(attr_str);
        } else if (isAttr("fontStyle")) {
            fontStyle = attr_str;
        } else if (isAttr("fontSrc")) {
            setFont(Font(getString(attr_str)));
        } else if (isAttr("fontSize")) {
            setFontSize(getDimenPx(attr_str));
        } else if (isAttr("fontColor")) {
            setFontColor(getColor(attr_str));
        } else if (isAttr("textAlignment")) {
            setTextAlignment(Alignment::parse(attr_str));
        }
    }

    if (!fontName.empty()) {
        Font::Style style = Font::Style::Any;

        if (!fontStyle.empty()) {
            if (fontStyle == "regular") {
                style = Font::Style::Regular;
            } else if (fontStyle == "bold") {
                style = Font::Style::Bold;
            } else if (fontStyle == "italic") {
                style = Font::Style::Italic;
            }
        }

        setFont(Font(fontName, style));
    }
}
}
