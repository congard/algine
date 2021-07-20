#ifndef ALGINE_PAINTER_H
#define ALGINE_PAINTER_H

#include <algine/core/font/FontRenderer.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/buffers/ArrayBuffer.h>
#include <algine/core/InputLayout.h>
#include <algine/core/painter/RoundRect.h>
#include <algine/core/painter/Paint.h>
#include <algine/core/Rect.h>
#include <algine/types.h>

namespace algine {
class Painter {
public:
    enum class RenderHint {
        Antialiasing = 0b01,
        OptimizeFontCache = 0b10
    };

    using RenderHints = uint;

public:
    Painter();
    ~Painter();

    void begin();
    void end();

    void setViewport(uint width, uint height);

    void setPaint(const Paint &paint);
    const Paint& getPaint() const;
    Paint& paint();

    void setFont(const Font &font, uint size);
    void setFont(const Font &font);
    void setFontSize(uint size);

    const Font& getFont() const;
    uint getFontSize() const;

    void setRenderHint(RenderHint renderHint, bool on = true);
    bool isRenderHintEnabled(RenderHint renderHint) const;
    void setRenderHints(RenderHints renderHints);
    RenderHints getRenderHints() const;

    void setTransform(const glm::mat4 &transform);
    const glm::mat4& getTransform() const;

    void drawLine(const PointF &p1, const PointF &p2);
    void drawLine(float x1, float y1, float x2, float y2);

    void drawTriangle(const PointF &p1, const PointF &p2, const PointF &p3);

    void drawRect(const RectF &rect);
    void drawRect(float x1, float y1, float x2, float y2);

    void drawRoundRect(const RoundRect &roundRect);
    void drawRoundRect(const RectF &rect, float r1, float r2, float r3, float r4);
    void drawRoundRect(const RectF &rect, float radius);

    void drawCircle(const PointF &origin, float radius);
    void drawCircle(float x, float y, float radius);

    void drawText(const std::u16string &text, const PointF &p);
    void drawText(const std::string &text, const PointF &p);
    void drawText(const std::string &text, float x, float y);

    void drawTexture(const Texture2DPtr &texture, const RectF &rect);
    void drawTexture(const Texture2DPtr &texture, const PointF &p);
    void drawTexture(const Texture2DPtr &texture, float x, float y);

    static void optimizeFontCache();
    static void clearFontCache();

private:
    Painter(const Painter&);
    Painter& operator=(const Painter&);

    void writeRectToBuffer(const RectF &rect);
    void writeTransformation(const ShaderProgramPtr &program);
    void writeProjection(const ShaderProgramPtr &program);
    void changeColor();
    void applyColor();
    void updateFontHash();
    void disownFontHash();

private:
    glm::mat4 m_projection;
    Paint m_paint;
    Ptr<ArrayBuffer> m_buffer;
    Ptr<InputLayout> m_layout;

private:
    Texture2DPtr m_colorTex;
    Color m_activeColor;
    RenderHints m_renderHints;
    glm::mat4 m_transform;

private:
    FontRenderer m_fontRenderer;
    unsigned long m_fontHash;

private:
    bool m_wasDepthTestEnabled;
    bool m_wasBlendingEnabled;
    int m_prevSrcAlphaBlendMode;

private:
    static ShaderProgramPtr m_fill;
    static ShaderProgramPtr m_circleFill;
    static ShaderProgramPtr m_roundRectFill;
    static ShaderProgramPtr m_textFill;

private:
    struct Character {
        Texture2DPtr texture;
        float bearingLeft;
        float bearingTop;
        float advance;
    };

    struct Characters {
        std::unordered_map<char16_t, Character> characters;
        int counter {0};
    };

    static std::unordered_map<unsigned long, Characters> m_characters;
};
}

#endif //ALGINE_PAINTER_H
