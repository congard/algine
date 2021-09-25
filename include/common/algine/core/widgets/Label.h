#ifndef ALGINE_LABEL_H
#define ALGINE_LABEL_H

#include <algine/core/widgets/Widget.h>
#include <algine/core/font/Font.h>
#include <algine/core/font/FontMetrics.h>

namespace algine {
class Label: public Widget {
public:
    Label();
    explicit Label(const std::string &text);

    void setText(const std::string &text);
    const std::string& getText() const;

    void setFont(const Font &font);
    void setFont(const Font &font, uint size);
    void setFontSize(uint size);
    void setFontColor(const Color &color);

    const Font& getFont() const;
    uint getFontSize() const;
    const Color& getFontColor() const;

    void setTextAlignment(uint alignment);
    uint getTextAlignment() const;

    void fromXML(const pugi::xml_node &node, const std::shared_ptr<IOSystem> &io) override;

protected:
    void onMeasure(int &width, int &height) override;
    void onLayout() override;
    void onDraw(Painter &painter) override;

private:
    std::string m_text;
    uint m_textAlignment;
    FontMetrics m_fontMetrics;
    Color m_fontColor;
    float m_textX;
    float m_textY;
};
}

#endif //ALGINE_LABEL_H
