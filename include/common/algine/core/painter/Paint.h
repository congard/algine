#ifndef ALGINE_PAINT_H
#define ALGINE_PAINT_H

#include <algine/core/Color.h>
#include <algine/core/MutableValue.h>
#include <algine/platform.h>

#include <glm/mat4x4.hpp>

namespace algine {
class Texture2D;

class AL_EXPORT Paint {
    friend class Painter;

public:
    enum class Source {
        Color,
        Texture,
        None
    };

public:
    Paint();
    explicit Paint(const Color &color);
    explicit Paint(Texture2D *texture);

    void setColor(const Color &color);
    void setTexture(Texture2D *texture);
    void setSource(Source source);
    void setTransform(const glm::mat4 &transform);

    const Color& getColor() const;
    Texture2D* getTexture() const;
    Source getSource() const;
    const glm::mat4& getTransform() const;

private:
    Color m_color;
    Texture2D *m_texture;
    Source m_source;
    MutableValue<glm::mat4> m_transform;
};
}

#endif //ALGINE_PAINT_H
