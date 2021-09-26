#include <algine/core/painter/Painter.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/Engine.h>
#include <algine/core/shader/ShaderCreator.h>

#include <glm/gtc/matrix_transform.hpp>

#include <codecvt>
#include <locale>

#include "PainterShaders.h"
#include "../djb2.h"

namespace algine {
ShaderProgramPtr Painter::m_fill;
ShaderProgramPtr Painter::m_circleFill;
ShaderProgramPtr Painter::m_roundRectFill;
ShaderProgramPtr Painter::m_textFill;

// <hash, loaded characters + counter>
// hash is generated using font name, width, height and style
std::unordered_map<unsigned long, Painter::Characters> Painter::m_characters;

Painter::Painter()
    : m_projection(),
      m_paint(),
      m_wasDepthTestEnabled(),
      m_wasBlendingEnabled(),
      m_prevSrcAlphaBlendMode(),
      m_colorTex(PtrMaker::make<Texture2D>()),
      m_activeColor(Color(0xff000000)),
      m_renderHints(0),
      m_transform(1.0f),
      m_opacity(1.0f),
      m_fontHash(0)
{
    // TODO: implement in the engine
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    m_layout = std::make_unique<InputLayout>();
    m_buffer = std::make_unique<ArrayBuffer>();

    InputAttributeDescription attribDescription;
    attribDescription.setLocation(0);
    attribDescription.setCount(4);
    attribDescription.setDataType(DataType::Float);
    attribDescription.setStride(4 * sizeof(float));
    attribDescription.setOffset(0);

    m_layout->bind();
    m_layout->addAttribute(attribDescription, m_buffer.get());
    m_layout->unbind();

    auto initProgram = [](ShaderProgramPtr &program, const ShaderPtr &vs, const char* fsSource) {
        ShaderCreator fsCreator(Shader::Fragment);
        fsCreator.setSource(fsSource);

        program = PtrMaker::make();
        program->attachShader(*vs);
        program->attachShader(*fsCreator.create());
        program->link();
        program->loadActiveLocations();
    };

    // init shaders if haven't been inited yet
    if (m_fill == nullptr) {
        ShaderCreator vsCreator(Shader::Vertex);
        vsCreator.setSource(PainterShaders::painter_vs);

        auto vs = vsCreator.create();

        initProgram(m_fill, vs, PainterShaders::fill_fs);
        initProgram(m_circleFill, vs, PainterShaders::circle_fill_fs);
        initProgram(m_roundRectFill, vs, PainterShaders::round_rect_fill_fs);
        initProgram(m_textFill, vs, PainterShaders::text_fill_fs);

        m_textFill->bind();
        m_textFill->setInt("symbol", 1);
    }

    m_colorTex->bind();
    m_colorTex->setFormat(Texture::RGBA32F);
    m_colorTex->setWidth(1);
    m_colorTex->setHeight(1);
    m_colorTex->setParams(std::map<uint, uint> {
            {Texture::WrapU, Texture::ClampToEdge},
            {Texture::WrapV, Texture::ClampToEdge},
            {Texture::MinFilter, Texture::Nearest},
            {Texture::MagFilter, Texture::Nearest}
    });

    changeColor();
}

Painter::~Painter() {
    disownFontHash();
}

void Painter::begin() {
    m_layout->bind();
    m_buffer->bind();

    // TODO: implement MSAA

    if (!isRenderHintEnabled(RenderHint::DisableStateSaving)) {
        m_wasDepthTestEnabled = Engine::isDepthTestEnabled();

        if (!isRenderHintEnabled(RenderHint::DisableAlphaBlending)) {
            m_wasBlendingEnabled = glIsEnabled(GL_BLEND); // TODO: to Engine
            glGetIntegerv(GL_BLEND_DST_ALPHA, &m_prevSrcAlphaBlendMode); // TODO: to Engine
        }
    }

    if (!isRenderHintEnabled(RenderHint::DisableAlphaBlending)) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    Engine::disableDepthTest();
}

void Painter::end() {
    if (!isRenderHintEnabled(RenderHint::DisableStateSaving)) {
        if (m_wasDepthTestEnabled) {
            Engine::enableDepthTest();
        }

        if (!isRenderHintEnabled(RenderHint::DisableAlphaBlending)) {
            if (!m_wasBlendingEnabled) {
                glDisable(GL_BLEND);
            }

            glBlendFunc(GL_SRC_ALPHA, m_prevSrcAlphaBlendMode);
        }
    }

    m_buffer->unbind();
    m_layout->unbind();
}

void Painter::setViewport(uint width, uint height) {
    m_projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
}

void Painter::setPaint(const Paint &paint) {
    m_paint = paint;
}

const Paint& Painter::getPaint() const {
    return m_paint;
}

Paint& Painter::paint() {
    return m_paint;
}

void Painter::setFont(const Font &font, uint size) {
    m_fontRenderer.setFont(font);
    m_fontRenderer.setFontSize(size);
    updateFontHash();
}

void Painter::setFont(const Font &font) {
    m_fontRenderer.setFont(font);
    updateFontHash();
}

void Painter::setFontSize(uint size) {
    m_fontRenderer.setFontSize(size);
    updateFontHash();
}

const Font& Painter::getFont() const {
    return m_fontRenderer.getFont();
}

uint Painter::getFontSize() const {
    return m_fontRenderer.getFontHeight();
}

template<typename T>
inline uint asRenderHint(T hint) {
    uint fl = static_cast<uint>(hint);
    return 1u << fl;
}

void Painter::setRenderHint(RenderHint renderHint, bool on) {
    uint hint = asRenderHint(renderHint);
    m_renderHints = on ? (m_renderHints | hint) : (m_renderHints & ~hint);
}

bool Painter::isRenderHintEnabled(RenderHint renderHint) const {
    uint hint = asRenderHint(renderHint);
    return m_renderHints & hint;
}

void Painter::setRenderHints(RenderHints renderHints) {
    m_renderHints = renderHints;
}

Painter::RenderHints Painter::getRenderHints() const {
    return m_renderHints;
}

void Painter::setTransform(const glm::mat4 &transform) {
    m_transform = transform;
}

const glm::mat4& Painter::getTransform() const {
    return m_transform;
}

void Painter::setOpacity(float opacity) {
    m_opacity = opacity;
}

float Painter::getOpacity() const {
    return m_opacity;
}

void Painter::drawLine(const PointF &p1, const PointF &p2) {
    float vertices[] = {
            p1.getX(), p1.getY(),  0.0f, 0.0f,
            p2.getX(), p2.getY(),  0.0f, 0.0f
    };

    // generate UV
    if (m_paint.getSource() == Paint::Source::Texture) {
        PointF m {std::min(p1.getX(), p2.getX()), std::min(p1.getY(), p2.getY())};
        PointF n {std::max(p1.getX(), p2.getX()), std::max(p1.getY(), p2.getY())};

        auto k = n - m;

        auto uv_p1 = p1 - m;
        uv_p1.x() /= k.getX();
        uv_p1.y() /= k.getY();

        auto uv_p2 = p2 - m;
        uv_p2.x() /= k.getX();
        uv_p2.y() /= k.getY();

        vertices[2] = uv_p1.getX();
        vertices[3] = 1.0f - uv_p1.getY();
        vertices[6] = uv_p2.getX();
        vertices[7] = 1.0f - uv_p2.getY();
    }

    if (m_buffer->size() < sizeof(vertices)) {
        m_buffer->setData(sizeof(vertices), vertices, Buffer::DynamicDraw);
    } else {
        m_buffer->updateData(0, sizeof(vertices), vertices);
    }

    applyColor();

    m_fill->bind();
    m_fill->setFloat("opacity", m_opacity);
    writeProjection(m_fill);
    writeTransformation(m_fill);

    Engine::drawArrays(0, 2, Engine::PolyType::Line);
}

void Painter::drawLine(float x1, float y1, float x2, float y2) {
    drawLine({x1, y1}, {x2, y2});
}

void Painter::drawTriangle(const PointF &p1, const PointF &p2, const PointF &p3) {
    glm::vec3 p1p2 = glm::vec3(p2.getX() - p1.getX(), p2.getY() - p1.getY(), 0.0f);
    glm::vec3 p2p3 = glm::vec3(p3.getX() - p2.getX(), p3.getY() - p2.getY(), 0.0f);
    glm::vec3 product = glm::cross(p2p3, p1p2);

    PointF a {p1};
    PointF b {p2};
    PointF c {p3};

    if (product.z < 0) {
        std::swap(a, c);
    }

    float vertices[] = {
            a.getX(), a.getY(),  0.0f, 0.0f,
            b.getX(), b.getY(),  0.0f, 0.0f,
            c.getX(), c.getY(),  0.0f, 0.0f,
    };

    // generate UV
    if (m_paint.getSource() == Paint::Source::Texture) {
        PointF m {
            std::min(a.getX(), std::min(b.getX(), c.getX())),
            std::min(a.getY(), std::min(b.getY(), c.getY()))
        };

        PointF n {
            std::max(a.getX(), std::max(b.getX(), c.getX())),
            std::max(a.getY(), std::max(b.getY(), c.getY()))
        };

        auto k = n - m;

        auto uv_a = a - m;
        uv_a.x() /= k.getX();
        uv_a.y() /= k.getY();

        auto uv_b = b - m;
        uv_b.x() /= k.getX();
        uv_b.y() /= k.getY();

        auto uv_c = c - m;
        uv_c.x() /= k.getX();
        uv_c.y() /= k.getY();

        vertices[2] = uv_a.getX();
        vertices[3] = 1.0f - uv_a.getY();
        vertices[6] = uv_b.getX();
        vertices[7] = 1.0f - uv_b.getY();
        vertices[10] = uv_c.getX();
        vertices[11] = 1.0f - uv_c.getY();
    }

    if (m_buffer->size() < sizeof(vertices)) {
        m_buffer->setData(sizeof(vertices), vertices, Buffer::DynamicDraw);
    } else {
        m_buffer->updateData(0, sizeof(vertices), vertices);
    }

    applyColor();

    m_fill->bind();
    m_fill->setFloat("opacity", m_opacity);
    writeProjection(m_fill);
    writeTransformation(m_fill);

    Engine::drawArrays(0, 3);
}

void Painter::drawRect(const RectF &rect) {
    writeRectToBuffer(rect);
    applyColor();

    m_fill->bind();
    m_fill->setFloat("opacity", m_opacity);
    writeProjection(m_fill);
    writeTransformation(m_fill);

    Engine::drawArrays(0, 4, Engine::PolyType::TriangleStrip);
}

void Painter::drawRect(float x1, float y1, float x2, float y2) {
    drawRect({{x1, y1}, {x2, y2}});
}

void Painter::drawRoundRect(const RoundRect &roundRect) {
    const RectF &rect = roundRect.getRect();

    float r1 = roundRect.getTopLeftCorner().getXRadius();
    float r2 = roundRect.getTopRightCorner().getXRadius();
    float r3 = roundRect.getBottomLeftCorner().getXRadius();
    float r4 = roundRect.getBottomRightCorner().getXRadius();

    float s1 = roundRect.getTopLeftCorner().getYRadius() / r1;
    float s2 = roundRect.getTopRightCorner().getYRadius() / r2;
    float s3 = roundRect.getBottomLeftCorner().getYRadius() / r3;
    float s4 = roundRect.getBottomRightCorner().getYRadius() / r4;

    writeRectToBuffer(rect);
    applyColor();

    m_roundRectFill->bind();
    m_roundRectFill->setFloat("opacity", m_opacity);
    m_roundRectFill->setVec4("p1p2",
        rect.getX(), rect.getY(),
        rect.getX() + rect.getWidth(), rect.getY() + rect.getHeight());
    m_roundRectFill->setVec4("radius", r1, r2, r3, r4);
    m_roundRectFill->setVec4("scale", s1, s2, s3, s4);
    m_roundRectFill->setBool("antialiasing", isRenderHintEnabled(RenderHint::Antialiasing));

    writeProjection(m_roundRectFill);
    writeTransformation(m_roundRectFill);

    Engine::drawArrays(0, 4, Engine::PolyType::TriangleStrip);
}

void Painter::drawRoundRect(const RectF &rect, float r1, float r2, float r3, float r4) {
    drawRoundRect({rect, r1, r2, r3, r4});
}

void Painter::drawRoundRect(const RectF &rect, float radius) {
    drawRoundRect({rect, radius});
}

void Painter::drawCircle(const PointF &origin, float radius) {
    writeRectToBuffer({
        origin.getX() - radius,
        origin.getY() - radius,
        radius * 2,
        radius * 2
    });
    applyColor();

    m_circleFill->bind();
    m_circleFill->setFloat("opacity", m_opacity);
    m_circleFill->setVec2("origin", origin.getX(), origin.getY());
    m_circleFill->setFloat("radius", radius);
    m_circleFill->setBool("antialiasing", isRenderHintEnabled(RenderHint::Antialiasing));

    writeProjection(m_circleFill);
    writeTransformation(m_circleFill);

    Engine::drawArrays(0, 4, Engine::PolyType::TriangleStrip);
}

void Painter::drawCircle(float x, float y, float radius) {
    drawCircle({x, y}, radius);
}

void Painter::drawText(std::u16string_view text, const PointF &p) {
    m_textFill->bind();
    m_textFill->setFloat("opacity", m_opacity);
    writeProjection(m_textFill);
    writeTransformation(m_textFill);

    constexpr float scale = 1.0f;

    float x = p.getX();
    float y = p.getY();

    auto &characters = m_characters[m_fontHash].characters;

    for (auto symbol : text) {
        // if symbol has not been loaded yet - load it
        if (characters.find(symbol) == characters.end()) {
            auto character = m_fontRenderer.getCharacter(symbol);

            Texture2DPtr tex = PtrMaker::make();
            tex->bind();
            tex->setFormat(Texture::Red8);
            tex->setWidth(character.width);
            tex->setHeight(character.height);
            tex->update(Texture::Red, DataType::UnsignedByte, character.data.array());

            tex->setParams(std::map<uint, uint> {
                    {Texture::WrapU, Texture::ClampToEdge},
                    {Texture::WrapV, Texture::ClampToEdge},
                    {Texture::MinFilter, Texture::Linear},
                    {Texture::MagFilter, Texture::Linear}
            });

            Character readyCharacter;
            readyCharacter.texture = tex;
            readyCharacter.bearingTop = static_cast<float>(character.bearingTop);
            readyCharacter.bearingLeft = static_cast<float>(character.bearingLeft);
            readyCharacter.advance = static_cast<float>(character.advance) / 64.0f;

            characters[symbol] = readyCharacter;
        }

        // render symbol

        const Character &character = characters[symbol];

        auto width = static_cast<float>(character.texture->getWidth());
        auto height = static_cast<float>(character.texture->getHeight());

        // s_ means "symbol" or "scaled"

        float s_x = x + character.bearingLeft * scale;
        float s_y = y + (height - character.bearingTop) * scale;

        float s_w = width * scale;
        float s_h = height * scale;

        float vertices[] = {
                s_x,        s_y,        0.0f, 0.0f,
                s_x + s_w,  s_y,        1.0f, 0.0f,
                s_x,        s_y - s_h,  0.0f, 1.0f,
                s_x + s_w,  s_y - s_h,  1.0f, 1.0f
        };

        applyColor();
        character.texture->use(1);

        if (m_buffer->size() < sizeof(vertices)) {
            m_buffer->setData(sizeof(vertices), vertices, Buffer::DynamicDraw);
        } else {
            m_buffer->updateData(0, sizeof(vertices), vertices);
        }

        Engine::drawArrays(0, 4, Engine::PolyType::TriangleStrip);

        x += character.advance * scale;
    }
}

void Painter::drawText(std::string_view text, const PointF &p) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utfConv;
    auto utf16 = utfConv.from_bytes(text.begin(), text.end());
    drawText(utf16, p);
}

void Painter::drawText(std::string_view text, float x, float y) {
    drawText(text, {x, y});
}

inline void writeTransformation(const ShaderProgramPtr &program,
    const glm::mat4 &transform, const glm::mat4 &paintTransform)
{
    program->setMat4("transformation", transform);
    program->setMat4("paintTransformation", paintTransform);
}

void Painter::drawTexture(const Texture2DPtr &texture, const RectF &rect) {
    writeRectToBuffer(rect);
    texture->use(0);

    m_fill->bind();
    m_fill->setFloat("opacity", m_opacity);
    writeProjection(m_fill);
    algine::writeTransformation(m_fill, m_transform, glm::mat4(1.0f));

    Engine::drawArrays(0, 4, Engine::PolyType::TriangleStrip);
}

void Painter::drawTexture(const Texture2DPtr &texture, const PointF &p) {
    texture->use(0);

    writeRectToBuffer({
        p.getX(),
        p.getY(),
        static_cast<float>(texture->getActualWidth()),
        static_cast<float>(texture->getActualHeight())
    });

    m_fill->bind();
    m_fill->setFloat("opacity", m_opacity);
    writeProjection(m_fill);
    algine::writeTransformation(m_fill, m_transform, glm::mat4(1.0f));

    Engine::drawArrays(0, 4, Engine::PolyType::TriangleStrip);
}

void Painter::drawTexture(const Texture2DPtr &texture, float x, float y) {
    drawTexture(texture, {x, y});
}

void Painter::optimizeFontCache() {
    for (auto it = m_characters.begin(); it != m_characters.end();) {
        if (it->second.counter == 0) {
            it = m_characters.erase(it);
        } else {
            ++it;
        }
    }
}

void Painter::clearFontCache() {
    m_characters = {};
}

void Painter::writeRectToBuffer(const RectF &rect) {
    float x = rect.getX();
    float y = rect.getY();
    float width = rect.getWidth();
    float height = rect.getHeight();

    float vertices[] = {
            x,          y + height,  0.0f, 0.0f,
            x + width,  y + height,  1.0f, 0.0f,
            x,          y,           0.0f, 1.0f,
            x + width,  y,           1.0f, 1.0f
    };

    if (m_buffer->size() < sizeof(vertices)) {
        m_buffer->setData(sizeof(vertices), vertices, Buffer::DynamicDraw);
    } else {
        m_buffer->updateData(0, sizeof(vertices), vertices);
    }
}

void Painter::writeTransformation(const ShaderProgramPtr &program) {
    algine::writeTransformation(program, m_transform, m_paint.getTransform());
}

void Painter::writeProjection(const ShaderProgramPtr &program) {
    program->setMat4("projection", m_projection);
}

void Painter::changeColor() {
    const Color &c = m_paint.getColor();
    float color[] = {c.redF(), c.greenF(), c.blueF(), c.alphaF()};

    m_colorTex->update(Texture::RGBA, DataType::Float, color);

    m_activeColor = c;
}

void Painter::applyColor() {
    switch (m_paint.getSource()) {
        case Paint::Source::Color:
            m_colorTex->use(0);

            if (m_activeColor != m_paint.getColor()) {
                changeColor();
            }

            break;
        case Paint::Source::Texture:
            m_paint.getTexture()->use(0);
            break;
        default: throw std::invalid_argument("Invalid paint source");
    }
}

void Painter::updateFontHash() {
    std::string fontName = m_fontRenderer.getFont().getName();
    std::string fontWidth = std::to_string(m_fontRenderer.getFontWidth());
    std::string fontHeight = std::to_string(m_fontRenderer.getFontHeight());
    std::string fontStyle = std::to_string(static_cast<uint>(m_fontRenderer.getFont().getStyle()));

    disownFontHash();

    m_fontHash = hash_djb2((unsigned char *) (fontName + " " + fontWidth + " " + fontHeight + " " + fontStyle).c_str());

    if (m_characters.find(m_fontHash) == m_characters.end()) {
        m_characters[m_fontHash] = Characters {};
    }

    m_characters[m_fontHash].counter++;
}

void Painter::disownFontHash() {
    if (auto it = m_characters.find(m_fontHash); it != m_characters.end()) {
        it->second.counter--;

        if (isRenderHintEnabled(RenderHint::OptimizeFontCache) && it->second.counter == 0) {
            m_characters.erase(it);
        }
    }
}
}
