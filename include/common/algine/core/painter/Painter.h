#ifndef ALGINE_PAINTER_H
#define ALGINE_PAINTER_H

#include <algine/core/font/FontRenderer.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/buffers/ArrayBuffer.h>
#include <algine/core/InputLayout.h>
#include <algine/core/painter/RoundRect.h>
#include <algine/core/painter/Paint.h>
#include <algine/core/math/Rect.h>
#include <algine/core/PtrView.h>
#include <algine/core/MutableValue.h>
#include <algine/types.h>

namespace algine {
class AL_EXPORT Painter: public Object {
public:
    enum class RenderHint {
        Antialiasing,
        OptimizeFontCache,
        DisableStateSaving,
        DisableAlphaBlending,
        ContinuousDrawing
    };

    using RenderHints = uint;

public:
    explicit Painter(Object *parent = defaultParent());
    ~Painter() override;

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

    void setOpacity(float opacity);
    float getOpacity() const;

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

    void drawText(std::u16string_view text, const PointF &p);
    void drawText(std::string_view text, const PointF &p);
    void drawText(std::string_view text, float x, float y);

    void drawTexture(Texture2D *texture, const RectF &rect);
    void drawTexture(Texture2D *texture, const PointF &p);
    void drawTexture(Texture2D *texture, float x, float y);

    bool isActive() const;

    static void optimizeFontCache();
    static void clearFontCache();

private:
    Painter(const Painter&);
    Painter& operator=(const Painter&);

    void bindFillProgram();
    void bindFillTexProgram();

    void writeRectToBuffer(const RectF &rect);
    void changeColor();
    void applyColor();
    void updateFontHash();
    void disownFontHash();

    template<bool forcibly, typename T, typename U>
    inline void write_mv(T &&setter, MutableValue<U> &value) {
        if constexpr(forcibly) {
            setter();
        } else {
            if (value.hasChanged()) {
                setter();
            }
        }
    }

    template<bool forcibly>
    inline void writeMat4(const char *name, MutableValue<glm::mat4> &value, ShaderProgram *program) {
        write_mv<forcibly>([&]() { program->setMat4(name, value); }, value);
    }

    template<bool forcibly>
    inline void writeFloat(const char *name, MutableValue<float> &value, ShaderProgram *program) {
        write_mv<forcibly>([&]() { program->setFloat(name, value); }, value);
    }

    template<bool forcibly>
    inline void writeTPMatrix(ShaderProgram *program) {
        if (forcibly || m_projection.hasChanged() || m_transform.hasChanged()) {
            program->setMat4("tpMatrix", m_projection.get() * m_transform.get());
        }
    }

    template<bool forcibly>
    inline void writePaintTransform(ShaderProgram *program) {
        writeMat4<forcibly>("paintTransformation", m_paint.m_transform, program);
    }

private:
    MutableValue<glm::mat4> m_projection;
    MutableValue<glm::mat4> m_transform;
    MutableValue<float> m_opacity;
    Paint m_paint;
    ArrayBuffer *m_buffer;
    InputLayout *m_layout;

    ShaderProgram *m_activeProgram;

private:
    Texture2D *m_colorTex;
    Color m_activeColor;
    RenderHints m_renderHints;

private:
    FontRenderer m_fontRenderer;
    unsigned long m_fontHash;

private:
    bool m_wasDepthTestEnabled;
    bool m_wasBlendingEnabled;
    int m_prevSrcAlphaBlendMode;

private:
    bool m_isActive;

private:
    // TODO: class PainterStorage?
    static ShaderProgram *m_fill;
    static ShaderProgram *m_circleFill;
    static ShaderProgram *m_roundRectFill;
    static ShaderProgram *m_textFill;

private:
    struct Character {
        Texture2D *texture;
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
