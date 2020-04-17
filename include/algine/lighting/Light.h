#ifndef ALGINE_LIGHT_H
#define ALGINE_LIGHT_H

#include <algine/types.h>
#include <algine/framebuffer.h>
#include <algine/object3d.h>

namespace algine {
namespace Lighting {
class Light: public translatable {
public:
    enum {
        TypeDirLight,
        TypePointLight
    };

    ~Light();

    void translate() override;

    virtual void initShadows(uint shadowMapWidth = 512, uint shadowMapHeight = 512) = 0;
    virtual void updateMatrix() = 0;
    virtual void begin() = 0;
    void end();

    void setKc(float kc);
    void setKl(float kl);
    void setKq(float kq);
    void setColor(const glm::vec3 &color);
    void setColor(float red, float green, float blue);

    float getKc();
    float getKl();
    float getKq();
    glm::vec3 getColor();

public:
    int type = -1;
    float m_kc; // constant term; usually kept at 1.0
    float m_kl; // linear term
    float m_kq; // quadratic term
    Framebuffer *shadowMapFb = nullptr;
    glm::mat4 m_lightProjection;
    glm::vec3 m_color;
};
}
}

#endif //ALGINE_LIGHT_H
