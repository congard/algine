#ifndef ALGINE_DIRLIGHT_H
#define ALGINE_DIRLIGHT_H

#include <algine/std/lighting/Light.h>

namespace algine {
namespace Lighting {
class DirLight: public Light, public rotatable {
public:
    explicit DirLight(uint rotatorType = Rotator::RotatorTypeSimple);
    ~DirLight();

    void orthoShadows(float left, float right, float bottom, float top, float near = 1.0f, float far = 32.0f);
    void perspectiveShadows(float fovy, float aspect, float near = 1.0f, float far = 32.0f);

    void initShadows(uint shadowMapWidth = 512, uint shadowMapHeight = 512) override;

    /// updates light space matrix
    void updateMatrix() override;
    void begin() override;

    void setMinBias(float minBias);
    void setMaxBias(float maxBias);

    float getMinBias() const;
    float getMaxBias() const;

public:
    Texture2D *shadowMap = nullptr;
    glm::mat4 m_lightSpace;
    float m_minBias = 0.005f, m_maxBias = 0.05f;
};
}
}

#endif //ALGINE_DIRLIGHT_H
