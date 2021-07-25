#ifndef ALGINE_PAINT_H
#define ALGINE_PAINT_H

#include <algine/core/texture/Texture2DPtr.h>
#include <algine/core/Color.h>

#include <glm/mat4x4.hpp>

namespace algine {
class Paint {
public:
    enum class Source {
        Color,
        Texture,
        None
    };

public:
    Paint();
    explicit Paint(const Color &color);
    explicit Paint(Texture2DPtr texture);

    void setColor(const Color &color);
    void setTexture(const Texture2DPtr &texture);
    void setSource(Source source);
    void setTransform(const glm::mat4 &transform);

    const Color& getColor() const;
    const Texture2DPtr& getTexture() const;
    Source getSource() const;
    const glm::mat4& getTransform() const;

private:
    Color m_color;
    Texture2DPtr m_texture;
    Source m_source;
    glm::mat4 m_transform;
};
}

#endif //ALGINE_PAINT_H
