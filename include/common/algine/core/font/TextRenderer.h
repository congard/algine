#ifndef ALGINE_TEXTRENDERER_H
#define ALGINE_TEXTRENDERER_H

#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/buffers/ArrayBuffer.h>
#include <algine/core/InputLayout.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/font/FontRenderer.h>

namespace algine {
class TextRenderer {
public:
    TextRenderer();

public:
    void setViewport(uint width, uint height);
    void setX(float x);
    void setY(float y);
    void setPos(float x, float y);
    void setPos(const glm::vec2 &pos);
    void setScale(float scale);
    void setColor(const glm::vec3 &color);
    void setText(const std::string &text);
    void setFont(const Font &font, uint size = 16);
    void setFontSize(uint size);

    void begin();
    void end();

    void render();

private:
    struct Character {
        Texture2DPtr texture;
        float bearingLeft;
        float bearingTop;
        long advance; // смещение до следующего глифа
    };

private:
    ShaderProgramPtr m_shader;
    Ptr<ArrayBuffer> m_buffer;
    Ptr<InputLayout> m_layout;

private:
    float m_scale;
    float m_x;
    float m_y;
    glm::vec3 m_color;
    glm::mat4 m_projection;

private:
    bool m_wasDepthTestEnabled;
    bool m_wasBlendingEnabled;
    int m_prevSrcAlphaBlendMode;

private:
    FontRenderer m_fontRenderer;
    std::unordered_map<char16_t, Character> m_characters;
    std::u16string m_text;
};
}

#endif //ALGINE_TEXTRENDERER_H
