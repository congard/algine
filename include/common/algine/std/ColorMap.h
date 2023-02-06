#ifndef ALGINE_COLORMAP_H
#define ALGINE_COLORMAP_H

#include <algine/core/texture/Texture2D.h>
#include <algine/core/Color.h>

#include <glm/vec2.hpp>

#include <map>

namespace algine {
class ColorMap {
private:
    struct _ColorComparator {
        bool operator()(const Color &lhs, const Color &rhs) const {
            return lhs.value() < rhs.value();
        }
    };

public:
    using Colors = std::map<Color, glm::ivec2, _ColorComparator>;

public:
    ColorMap();
    ColorMap(uint width, uint height, uint format = Texture::RGBA8UI);
    explicit ColorMap(glm::ivec2 size, uint format = Texture::RGBA8UI);

    void create(glm::ivec2 size, uint format = Texture::RGBA8UI);
    void update();

    void setColors(const Colors &colors);
    void setColor(const Color &color, const glm::ivec2 &uv);

    const Colors& getColors() const;
    const glm::ivec2& getUV(const Color &color) const;
    const Color& getColor(const glm::ivec2 &uv) const;

    glm::vec2 getNormalizedUV(const Color &color) const;
    glm::vec2 normalizeUV(const glm::ivec2 &uv) const;
    glm::ivec2 denormalizeUV(const glm::vec2 &uv) const;

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
    bool exists(const glm::ivec2 &uv) const;

    const Texture2DPtr& get() const;

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
    Texture2DPtr m_texture;
    Colors m_colors;
};
}

#endif //ALGINE_COLORMAP_H
