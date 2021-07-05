#include <algine/core/font/TextRenderer.h>

#include <algine/core/shader/ShaderCreator.h>
#include <algine/core/shader/ShaderProgramCreator.h>

#include <algine/core/PtrMaker.h>
#include <algine/core/Engine.h>

#include <glm/gtc/matrix_transform.hpp>

#include <codecvt>
#include <locale>

#include "TextRendererShaders.h"

using namespace std;

namespace algine {
bool TextRenderer::m_cacheOptimization = true;

// <hash, loaded characters + counter>
// hash is generated using font name, width, height and style
unordered_map<unsigned long, TextRenderer::Characters> TextRenderer::m_characters;

// Source: http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash_djb2(unsigned char *str) {
    unsigned long hash = 5381;
    int c;

    while (c = *str++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

TextRenderer::TextRenderer()
    : m_scale(1.0f),
      m_x(0.0f),
      m_y(0.0f),
      m_color(0.0),
      m_projection(glm::ortho(0.0f, 512.0f, 0.0f, 512.0f)),
      m_wasDepthTestEnabled(),
      m_wasBlendingEnabled(),
      m_prevSrcAlphaBlendMode(),
      m_hash(0)
{
    constexpr auto programName = "@algine/TextRenderer";

    if (const auto &program = ShaderProgram::getByName(programName); program != nullptr) {
        m_shader = program;
    } else {
        ShaderCreator text_vertex(Shader::Vertex);
        text_vertex.setSource(TextRendererShaders::vertexShader);

        ShaderCreator text_fragment(Shader::Fragment);
        text_fragment.setSource(TextRendererShaders::fragmentShader);

        ShaderProgramCreator textShaderProgramCreator;
        textShaderProgramCreator.setAccess(Creator::Access::Public);
        textShaderProgramCreator.setName(programName);
        textShaderProgramCreator.addShader(text_vertex);
        textShaderProgramCreator.addShader(text_fragment);

        m_shader = textShaderProgramCreator.get();
        m_shader->loadActiveLocations();
    }

    // TODO: implement in the engine
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    m_layout = PtrMaker::make();
    m_buffer = PtrMaker::make();

    m_buffer->bind();
    m_layout->bind();
    m_buffer->setData(sizeof(float) * 4 * 4, nullptr, Buffer::DynamicDraw);

    InputAttributeDescription attribDescription;
    attribDescription.setLocation(0);
    attribDescription.setCount(4);
    attribDescription.setDataType(DataType::Float);
    attribDescription.setStride(4 * sizeof(float));
    attribDescription.setOffset(0);

    m_layout->addAttribute(attribDescription, m_buffer.get());

    //m_buffer->unbind(); // already done by m_layout->addAttribute call
    m_layout->unbind();
}

TextRenderer::~TextRenderer() {
    disownHash();
}

void TextRenderer::setViewport(uint width, uint height) {
    m_projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
}

void TextRenderer::setX(float x) {
    m_x = x;
}

void TextRenderer::setY(float y) {
    m_y = y;
}

void TextRenderer::setPos(float x, float y) {
    m_x = x;
    m_y = y;
}

void TextRenderer::setPos(const glm::vec2 &pos) {
    m_x = pos.x;
    m_y = pos.y;
}

void TextRenderer::setScale(float scale) {
    m_scale = scale;
}

void TextRenderer::setColor(const Color &color) {
    m_color = color;
}

void TextRenderer::setText(const std::string &text) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> utfConv;
    m_text = utfConv.from_bytes(text);
}

void TextRenderer::setText(const std::u16string &text) {
    m_text = text;
}

void TextRenderer::setFont(const Font &font, uint size) {
    m_fontRenderer.setFont(font);
    m_fontRenderer.setFontHeight(size);
    updateHash();
}

void TextRenderer::setFontSize(uint size) {
    m_fontRenderer.setFontHeight(size);
    updateHash();
}

float TextRenderer::getX() const {
    return m_x;
}

float TextRenderer::getY() const {
    return m_y;
}

float TextRenderer::getScale() const {
    return m_scale;
}

const Color& TextRenderer::getColor() const {
    return m_color;
}

const std::u16string &TextRenderer::getText() const {
    return m_text;
}

const Font& TextRenderer::getFont() const {
    return m_fontRenderer.getFont();
}

uint TextRenderer::getFontSize() const {
    return m_fontRenderer.getFontHeight();
}

void TextRenderer::begin() {
    m_layout->bind();
    m_buffer->bind();
    m_shader->bind();

    m_shader->setMat4("projection", m_projection);

    m_wasDepthTestEnabled = Engine::isDepthTestEnabled();
    m_wasBlendingEnabled = glIsEnabled(GL_BLEND); // TODO: to Engine
    glGetIntegerv(GL_BLEND_DST_ALPHA, &m_prevSrcAlphaBlendMode); // TODO: to Engine

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Texture2D::activateSlot(0);
}

void TextRenderer::end() {
    if (!m_wasBlendingEnabled) {
        glDisable(GL_BLEND);
    }

    if (m_wasDepthTestEnabled) {
        Engine::enableDepthTest();
    }

    glBlendFunc(GL_SRC_ALPHA, m_prevSrcAlphaBlendMode);

    m_shader->unbind();
    m_buffer->unbind();
    m_layout->unbind();
}

void TextRenderer::render() {
    m_shader->setVec4("charColor", m_color.redF(), m_color.greenF(), m_color.blueF(), m_color.alphaF());

    float x = m_x;
    float y = m_y;

    auto &characters = m_characters[m_hash].characters;

    for (auto symbol : m_text) {
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
            readyCharacter.advance = character.advance;

            characters[symbol] = readyCharacter;
        }

        // render symbol

        const Character &character = characters[symbol];

        auto width = static_cast<float>(character.texture->getWidth());
        auto height = static_cast<float>(character.texture->getHeight());

        // s_ means "symbol" or "scaled"

        float s_x = x + character.bearingLeft * m_scale;
        float s_y = y + (height - character.bearingTop) * m_scale;

        float s_w = width * m_scale;
        float s_h = height * m_scale;

        float vertices[] = {
                s_x,        s_y,        0.0f, 1.0f,
                s_x + s_w,  s_y,        1.0f, 1.0f,
                s_x,        s_y - s_h,  0.0f, 0.0f,
                s_x + s_w,  s_y - s_h,  1.0f, 0.0f
        };

        character.texture->bind();

        m_buffer->updateData(0, sizeof(vertices), vertices);

        Engine::drawArrays(0, 4, Engine::PolyType::TriangleStrip);

        x += static_cast<float>(character.advance >> 6) * m_scale;
    }
}

void TextRenderer::setAutoCacheOptimization(bool optimization) {
    m_cacheOptimization = optimization;
}

void TextRenderer::optimizeCache() {
    for (auto it = m_characters.begin(); it != m_characters.end();) {
        if (it->second.counter == 0) {
            it = m_characters.erase(it);
        } else {
            ++it;
        }
    }
}

void TextRenderer::clearCache() {
    m_characters = {};
}

void TextRenderer::updateHash() {
    string fontName = m_fontRenderer.getFont().getName();
    string fontWidth = to_string(m_fontRenderer.getFontWidth());
    string fontHeight = to_string(m_fontRenderer.getFontHeight());
    string fontStyle = to_string(static_cast<uint>(m_fontRenderer.getFont().getStyle()));

    disownHash();

    m_hash = hash_djb2((unsigned char *) (fontName + " " + fontWidth + " " + fontHeight + " " + fontStyle).c_str());

    if (m_characters.find(m_hash) == m_characters.end()) {
        m_characters[m_hash] = Characters {};
    }

    m_characters[m_hash].counter++;
}

void TextRenderer::disownHash() {
    if (auto it = m_characters.find(m_hash); it != m_characters.end()) {
        it->second.counter--;

        if (m_cacheOptimization && it->second.counter == 0) {
            m_characters.erase(it);
        }
    }
}
}
