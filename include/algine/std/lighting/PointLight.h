#ifndef ALGINE_POINTLIGHT_H
#define ALGINE_POINTLIGHT_H

#include <algine/std/lighting/Light.h>

namespace algine {
class PointLight: public Light {
public:
    PointLight();
    ~PointLight();

    void initShadows(uint shadowMapWidth = 512, uint shadowMapHeight = 512) override;
    void updateMatrix() override;
    void begin() override;

    void perspectiveShadows();

    void setFar(float far);
    void setNear(float near);
    void setBias(float bias);

    float getFar() const;
    float getNear() const;
    float getBias() const;

public:
    TextureCube *shadowMap = nullptr;
    glm::mat4 m_lightSpaceMatrices[6];
    float m_far = 32.0f, m_near = 1.0f;
    float m_bias = 0.4f;

    static const glm::mat4 m_lightViews[6];
};
}

#endif //ALGINE_POINTLIGHT_H
