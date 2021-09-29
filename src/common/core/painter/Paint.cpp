#include <algine/core/painter/Paint.h>

#include <utility>

namespace algine {
Paint::Paint()
    : m_color(0xff000000),
      m_texture(nullptr),
      m_source(Source::None),
      m_transform(glm::mat4(1.0f)) {}

Paint::Paint(const Color &color)
    : m_color(color),
      m_texture(nullptr),
      m_source(Source::Color),
      m_transform(glm::mat4(1.0f)) {}

Paint::Paint(Texture2DPtr texture)
    : m_color(0xff000000),
      m_texture(std::move(texture)),
      m_source(Source::Texture),
      m_transform(glm::mat4(1.0f)) {}

void Paint::setColor(const Color &color) {
    m_source = Source::Color;
    m_color = color;
}

void Paint::setTexture(const Texture2DPtr &texture) {
    m_source = Source::Texture;
    m_texture = texture;
}

void Paint::setSource(Source source) {
    m_source = source;
}

void Paint::setTransform(const glm::mat4 &transform) {
    m_transform = transform;
}

const Color& Paint::getColor() const {
    return m_color;
}

const Texture2DPtr& Paint::getTexture() const {
    return m_texture;
}

Paint::Source Paint::getSource() const {
    return m_source;
}

const glm::mat4& Paint::getTransform() const {
    return m_transform.read();
}
}
