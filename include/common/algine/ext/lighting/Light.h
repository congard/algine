#ifndef ALGINE_LIGHT_H
#define ALGINE_LIGHT_H

#include <algine/types.h>
#include <algine/core/Framebuffer.h>
#include <algine/std/Translatable.h>

namespace algine {
class Light: public Translatable {
    friend class LightingManager;

public:
    enum class Type {
        Dir,
        Point
    };

public:
    ~Light();

    void translate() override;

    virtual void initShadows(uint shadowMapWidth, uint shadowMapHeight) = 0;
    virtual void updateMatrix() = 0;
    virtual void begin() = 0;
    void end();

    void setKc(float kc);
    void setKl(float kl);
    void setKq(float kq);
    void setColor(const glm::vec3 &color);
    void setColor(float red, float green, float blue);

    float getKc() const;
    float getKl() const;
    float getKq() const;
    glm::vec3 getColor() const;

    Framebuffer* getShadowFramebuffer() const;
    const glm::mat4& getLightProjection() const;

protected:
    Type m_type;
    float m_kc; // constant term; usually kept at 1.0
    float m_kl; // linear term
    float m_kq; // quadratic term
    Framebuffer *m_shadowFb = nullptr;
    glm::mat4 m_lightProjection;
    glm::vec3 m_color;
};
}

#endif //ALGINE_LIGHT_H
