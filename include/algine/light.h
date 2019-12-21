/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My github: https://github.com/congard
 * @author congard
 */

#ifndef ALGINE_LIGHT_H
#define ALGINE_LIGHT_H

#include <algine/types.h>
#include <algine/model.h>
#include <algine/shader.h>
#include <algine/texture.h>
#include <algine/framebuffer.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace algine {
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

// in setters we don't using stride to get uniform location
// i.e location of myStruct[1].item = location of myStruct[0].item + elementsInStruct(myStruct)
// instead we loading it's location from shader's hash map
// because of https://stackoverflow.com/questions/57535432/glsl-array-of-struct-members-locations

/**
 * <b>Sets lighting values in shaders</b><br>
 * setVar sets value in Light Shader<br>
 * setShadowShaderVar sets value in Shadow Shader
 */
class LightDataSetter {
public:
    enum {
        DirLightsCount, PointLightsCount,
        Kc, Kl, Kq, Pos, Color, ShadowMap, // common
        MinBias, MaxBias, LightMatrix, // dir
        FarPlane, Bias, ShadowShaderPos, ShadowShaderFarPlane, ShadowShaderMatrices // point
    };

    LightDataSetter();
    LightDataSetter(const LightDataSetter &src); // copy constructor
    LightDataSetter(LightDataSetter &&src) noexcept; // move constructor
    LightDataSetter& operator=(const LightDataSetter &rhs); // copy operator
    LightDataSetter& operator=(LightDataSetter &&rhs) noexcept; // move operator
    ~LightDataSetter();

    void indexDirLightLocations(ShaderProgram *lightShader, uint lightsCount);
    void indexPointLightLocations(ShaderProgram *lightShader, ShaderProgram *shadowShader, uint lightsCount); // shadowShader can be nullptr

    void setDirLightsCount(uint count);
    void setPointLightsCount(uint count);

    void setKc(const Light &light, uint index);
    void setKl(const Light &light, uint index);
    void setKq(const Light &light, uint index);
    void setPos(const Light &light, uint index);
    void setColor(const Light &light, uint index);

    void setShadowMap(const DirLight &light, uint index, uint textureSlot);
    void setMinBias(const DirLight &light, uint index);
    void setMaxBias(const DirLight &light, uint index);
    void setLightMatrix(const DirLight &light, uint index);

    void setShadowMap(const PointLight &light, uint index, uint textureSlot);
    void setFarPlane(const PointLight &light, uint index);
    void setBias(const PointLight &light, uint index);
    void setShadowShaderPos(const PointLight &light);
    void setShadowShaderFarPlane(const PointLight &light);
    void setShadowShaderMatrices(const PointLight &light);

    int getLocation(uint obj, uint lightType, uint lightIndex);

private:
    struct LightLocations {
        int kc, kl, kq, pos, color, shadowMap;
    };

    struct DirLightLocations: public LightLocations {
        int minBias, maxBias, lightMatrix;
    };

    struct PointLightLocations: public LightLocations {
        int farPlane, bias;
    };

    int dirLightsCount = -1, pointLightsCount = -1;
    int shadowShaderPos = -1, shadowShaderFarPlane = -1, shadowShaderMatrices = -1; // point light shadow shader locations
    std::vector<LightLocations*> lightLocations[2]; // DirLightLocations, PointLightLocations

    void swap(LightDataSetter &other);
    void clearDirLightsLocations();
    void clearPointLightsLocations();
};

class PointLamp : public PointLight {
public:
    Model *mptr = nullptr;

    void setPos(const glm::vec3 &pos);
};

class DirLamp : public DirLight {
public:
    Model *mptr = nullptr;

    void setPos(const glm::vec3 &pos);
};

} /* namespace algine */

#endif // ALGINE_LIGHT_H
