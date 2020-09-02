#ifndef ALGINE_DIRLIGHT_H
#define ALGINE_DIRLIGHT_H

#include <algine/std/lighting/Light.h>
#include <algine/std/Rotatable.h>

namespace algine {
class DirLight: public Light, public Rotatable {
    friend class LightingManager;

public:
    explicit DirLight(uint rotatorType = Rotator::RotatorTypeSimple);
    ~DirLight();

    void orthoShadows(float left, float right, float bottom, float top, float near = 1.0f, float far = 32.0f);
    void perspectiveShadows(float fovy, float aspect, float near = 1.0f, float far = 32.0f);

    void initShadows(uint shadowMapWidth, uint shadowMapHeight) override;

    /// updates light space matrix
    void updateMatrix() override;
    void begin() override;

    void setMinBias(float minBias);
    void setMaxBias(float maxBias);

    float getMinBias() const;
    float getMaxBias() const;

    Texture2D* getShadowMap() const;
    const glm::mat4& getLightSpaceMatrix() const;

private:
    Texture2D *m_shadowMap = nullptr;
    glm::mat4 m_lightSpace;
    float m_minBias = 0.005f, m_maxBias = 0.05f;
};
}

#endif //ALGINE_DIRLIGHT_H
