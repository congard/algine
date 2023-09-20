#ifndef ALGINE_COLORMAP_H
#define ALGINE_COLORMAP_H

#include <algine/core/texture/Texture2D.h>
#include <algine/core/Object.h>
#include <algine/core/Color.h>

#include <glm/vec2.hpp>

#include <map>

namespace algine {
class AL_EXPORT ColorMap: public Object {
private:
    struct ColorComparator {
        bool operator()(glm::ivec2 lhs, glm::ivec2 rhs) const;
    };

public:
    using Colors = std::map<glm::ivec2, Color, ColorComparator>;

public:
    explicit ColorMap(Object *parent = defaultParent());
    ColorMap(uint width, uint height, uint format = Texture::RGBA8UI, Object *parent = defaultParent());
    explicit ColorMap(glm::ivec2 size, uint format = Texture::RGBA8UI, Object *parent = defaultParent());

    void create(glm::ivec2 size, uint format = Texture::RGBA8UI);
    void update();

    void setColors(const Colors &colors);
    void setColor(glm::ivec2 uv, const Color &color);

    const Colors& getColors() const;
    const glm::ivec2& getUV(const Color &color) const;
    const Color& getColor(glm::ivec2 uv) const;

    glm::vec2 getNormalizedUV(const Color &color) const;
    glm::vec2 normalizeUV(glm::ivec2 uv) const;
    glm::ivec2 denormalizeUV(glm::vec2 uv) const;

    /**
     * This function can be useful on some platforms,
     * where due to rounding you may get unexpected
     * or even "blinking" color
     * <br>
     * <br><b>Usage:</b>
     * <br><code>auto uv = map.getNormalizedUV(color) + map.getCenterOffset();</code>
     * @return offset to the cell's center
     */
    glm::vec2 getCenterOffset() const;

    void setWidth(uint width);
    void setHeight(uint height);
    void setFormat(uint format);

    uint getWidth() const;
    uint getHeight() const;
    uint getFormat() const;

    bool exists(const Color &color) const;
    bool exists(glm::ivec2 uv) const;

    Texture2D* get() const;

    /**
     * Static version of <code>normalizeUV</code>
     * @param uv
     * @param size
     * @return normalized UV, i.e UV in range [0, 1]
     */
    static glm::vec2 normalizeUV(glm::ivec2 uv, glm::ivec2 size);

    /**
     * Static version of <code>denormalizeUV</code>
     * @param uv
     * @param size
     * @return denormalized UV, i.e UV in range {[0, width], [0, height]}
     */
    static glm::ivec2 denormalizeUV(glm::vec2 uv, glm::ivec2 size);

    /**
     * Static version of <code>getCenterOffset</code>
     * @param size
     * @return offset to the cell's center
     */
    static glm::vec2 getCenterOffset(glm::ivec2 size);

private:
    Texture2D *m_texture;
    Colors m_colors;
};
}

#endif //ALGINE_COLORMAP_H
