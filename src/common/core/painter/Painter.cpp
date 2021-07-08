#include <algine/core/painter/Painter.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/Engine.h>
#include <algine/core/shader/ShaderCreator.h>

#include <glm/gtc/matrix_transform.hpp>

#include "PainterShaders.h"

namespace algine {
ShaderProgramPtr Painter::m_fill;
ShaderProgramPtr Painter::m_circleFill;
ShaderProgramPtr Painter::m_roundRectFill;

Painter::Painter()
    : m_projection(),
      m_paint(),
      m_textRenderer(PtrMaker::make<TextRenderer>()),
      m_wasDepthTestEnabled(),
      m_wasBlendingEnabled(),
      m_prevSrcAlphaBlendMode(),
      m_colorTex(PtrMaker::make<Texture2D>()),
      m_activeColor(Color(0xff000000)),
      m_renderHints(0),
      m_transform(1.0f)
{
    m_layout = PtrMaker::make();
    m_buffer = PtrMaker::make();

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

void Painter::begin() {
    m_layout->bind();
    m_buffer->bind();

    m_wasDepthTestEnabled = Engine::isDepthTestEnabled();
    Engine::disableDepthTest();

    // TODO: implement MSAA

    m_wasBlendingEnabled = glIsEnabled(GL_BLEND); // TODO: to Engine
    glGetIntegerv(GL_BLEND_DST_ALPHA, &m_prevSrcAlphaBlendMode); // TODO: to Engine

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Texture::activateSlot(0);

    m_fill->bind();
    m_fill->setMat4("projection", m_projection);

    m_circleFill->bind();
    m_circleFill->setMat4("projection", m_projection);

    m_roundRectFill->bind();
    m_roundRectFill->setMat4("projection", m_projection);
}

void Painter::end() {
    if (m_wasDepthTestEnabled) {
        Engine::enableDepthTest();
    }

    if (!m_wasBlendingEnabled) {
        glDisable(GL_BLEND);
    }

    glBlendFunc(GL_SRC_ALPHA, m_prevSrcAlphaBlendMode);

    m_buffer->unbind();
    m_layout->unbind();
}

void Painter::setViewport(uint width, uint height) {
    m_projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
    m_textRenderer->setViewport(width, height);
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
    m_textRenderer->setFont(font, size);
}

void Painter::setFont(const Font &font) {
    m_textRenderer->setFont(font, m_textRenderer->getFontSize());
}

void Painter::setFontSize(uint size) {
    m_textRenderer->setFontSize(size);
}

const Font& Painter::getFont() const {
    return m_textRenderer->getFont();
}

uint Painter::getFontSize() const {
    return m_textRenderer->getFontSize();
}

void Painter::setRenderHint(RenderHint renderHint, bool on) {
    uint hint = static_cast<uint>(renderHint);
    m_renderHints = on ? (m_renderHints | hint) : (m_renderHints & ~hint);
}

bool Painter::isRenderHintEnabled(RenderHint renderHint) const {
    uint hint = static_cast<uint>(renderHint);
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
        vertices[3] = uv_p1.getY();
        vertices[6] = uv_p2.getX();
        vertices[7] = uv_p2.getY();
    }

    if (m_buffer->size() < sizeof(vertices)) {
        m_buffer->setData(sizeof(vertices), vertices, Buffer::DynamicDraw);
    } else {
        m_buffer->updateData(0, sizeof(vertices), vertices);
    }

    applyColor();

    m_fill->bind();
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
        vertices[3] = uv_a.getY();
        vertices[6] = uv_b.getX();
        vertices[7] = uv_b.getY();
        vertices[10] = uv_c.getX();
        vertices[11] = uv_c.getY();
    }

    if (m_buffer->size() < sizeof(vertices)) {
        m_buffer->setData(sizeof(vertices), vertices, Buffer::DynamicDraw);
    } else {
        m_buffer->updateData(0, sizeof(vertices), vertices);
    }

    applyColor();

    m_fill->bind();
    writeTransformation(m_fill);

    Engine::drawArrays(0, 3);
}

void Painter::drawRect(const RectF &rect) {
    writeRectToBuffer(rect);
    applyColor();

    m_fill->bind();
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
    m_roundRectFill->setVec4("p1p2",
        rect.getX(), rect.getY(),
        rect.getX() + rect.getWidth(), rect.getY() + rect.getHeight());
    m_roundRectFill->setVec4("radius", r1, r2, r3, r4);
    m_roundRectFill->setVec4("scale", s1, s2, s3, s4);
    m_roundRectFill->setBool("antialiasing", isRenderHintEnabled(RenderHint::Antialiasing));

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
    m_circleFill->setVec2("origin", origin.getX(), origin.getY());
    m_circleFill->setFloat("radius", radius);
    m_circleFill->setBool("antialiasing", isRenderHintEnabled(RenderHint::Antialiasing));

    writeTransformation(m_circleFill);

    Engine::drawArrays(0, 4, Engine::PolyType::TriangleStrip);
}

void Painter::drawCircle(float x, float y, float radius) {
    drawCircle({x, y}, radius);
}

void Painter::drawText(const std::string &text, const PointF &p) {
    m_textRenderer->setColor(m_paint.getColor());
    m_textRenderer->setPos(p.getX(), p.getY());
    m_textRenderer->setText(text);
    m_textRenderer->begin();
    m_textRenderer->render();
    m_textRenderer->end();

    m_layout->bind();
    m_buffer->bind();
}

void Painter::drawText(const std::string &text, float x, float y) {
    drawText(text, {x, y});
}

void Painter::drawTexture(const Texture2DPtr &texture, const RectF &rect) {
    writeRectToBuffer(rect);
    texture->bind();

    m_fill->bind();

    Engine::drawArrays(0, 4, Engine::PolyType::TriangleStrip);
}

void Painter::drawTexture(const Texture2DPtr &texture, const PointF &p) {
    texture->bind();

    writeRectToBuffer({
        p.getX(),
        p.getY(),
        static_cast<float>(texture->getActualWidth()),
        static_cast<float>(texture->getActualHeight())
    });

    m_fill->bind();

    Engine::drawArrays(0, 4, Engine::PolyType::TriangleStrip);
}

void Painter::drawTexture(const Texture2DPtr &texture, float x, float y) {
    drawTexture(texture, {x, y});
}

void Painter::writeRectToBuffer(const RectF &rect) {
    float x = rect.getX();
    float y = rect.getY();
    float width = rect.getWidth();
    float height = rect.getHeight();

    float vertices[] = {
            x,          y + height,  0.0f, 1.0f,
            x + width,  y + height,  1.0f, 1.0f,
            x,          y,           0.0f, 0.0f,
            x + width,  y,           1.0f, 0.0f
    };

    if (m_buffer->size() < sizeof(vertices)) {
        m_buffer->setData(sizeof(vertices), vertices, Buffer::DynamicDraw);
    } else {
        m_buffer->updateData(0, sizeof(vertices), vertices);
    }
}

void Painter::writeTransformation(const ShaderProgramPtr &program) {
    program->setMat4("transformation", m_transform);
    program->setMat4("paintTransformation", m_paint.getTransform());
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
            m_colorTex->bind();

            if (m_activeColor != m_paint.getColor()) {
                changeColor();
            }

            break;
        case Paint::Source::Texture:
            m_paint.getTexture()->bind();
            break;
        default: throw std::invalid_argument("Invalid paint source");
    }
}
}
