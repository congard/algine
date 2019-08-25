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
#include <algine/shader_program.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace algine {
class Light {
public:
    enum {
        TypeDirLight,
        TypePointLight
    };

    int type = -1;

    glm::vec3 pos, color;
    float kc; // constant term; usually kept at 1.0
    float kl; // linear term
    float kq; // quadratic term
    usize shadowMapWidth, shadowMapHeight;
    uint depthMapFBO, depthMap;
    glm::mat4 lightProjection;

    virtual void updateMatrices();
    virtual void initShadowMapping(usize shadowMapWidth, usize shadowMapHeight);
    virtual void createDepthMapFBO();
    virtual void createDepthMap();
    virtual void configureDepthMap();
    virtual void begin();
	virtual void end();

    Light();
    ~Light();
};

class DirLight : public Light {
public:
    glm::mat4 lightView, lightSpace;
    glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    float minBias = 0.005f, maxBias = 0.05f;

    DirLight();

    void orthoShadowMapping(float left, float right, float bottom, float top, float near = 1.0f, float far = 32.0f);
    void perspectiveShadowMapping(float fovy, float aspect, float near = 1.0f, float far = 32.0f);
    void viewMatrixLookAt();
    void updateLightSpaceMatrix();
    void updateMatrices() override;
    void configureDepthMap() override;
    void begin() override;
	void end() override;
};

class PointLight : public Light {
public:
    // far plane
    float far = 32.0f;
    float bias = 0.4f;
    glm::mat4 lightViews[6], lightSpaceMatrices[6];

    PointLight();

    void updateMatrices() override;
    void configureDepthMap() override;
    void begin() override;
	void end() override;
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
