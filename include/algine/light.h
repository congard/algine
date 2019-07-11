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
#include <algine/algine_structs.h>
#include <algine/model.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace algine {
struct Light {
    glm::vec3 pos, color;
    float kc; // constant term; usually kept at 1.0
    float kl; // linear term
    float kq; // quadratic term
    CShader *cs = nullptr;
    SShader *ss = nullptr;
    usize index = 0;
    usize textureStartId = 6; // normal, reflectionStrength, jitter, ambient, diffuse, specular - last id is 5
    usize shadowMapWidth, shadowMapHeight;
    uint depthMapFBO, depthMap;
    glm::mat4 lightProjection;

    virtual void push_pos();
    virtual void push_color();
    virtual void push_kc();
    virtual void push_kl();
    virtual void push_kq();
    virtual void push_shadowMap();
    virtual void pushAll();

    virtual void updateMatrices();

    virtual void initShadowMapping(const usize shadowMapWidth, const usize shadowMapHeight);

    virtual void createDepthMapFBO();

    virtual void createDepthMap();

    virtual void configureDepthMap();

    virtual void begin();
		
	virtual void end();

    Light();

    ~Light();
};

struct DirLight : public Light {
    glm::mat4 lightView, lightSpace;
    glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    float minBias = 0.005f, maxBias = 0.05f;

    virtual void push_pos();
    virtual void push_color();
    virtual void push_kc();
    virtual void push_kl();
    virtual void push_kq();
    virtual void push_lightMatrix();
    virtual void push_minBias();
    virtual void push_maxBias();
    virtual void push_shadowMap();
    virtual void pushAll();

    virtual void orthoShadowMapping(const float left, const float right, const float bottom, const float top, const float near = 1.0f, const float far = 32.0f);

    virtual void perspectiveShadowMapping(const float fovy, const float aspect, const float near = 1.0f, const float far = 32.0f);

    virtual void viewMatrixLookAt();

    virtual void updateLightSpaceMatrix();

    virtual void updateMatrices();

    virtual void configureDepthMap();

    virtual void begin();
		
	virtual void end();
};

struct PointLight : public Light {
    // far plane
    float far = 32.0f;
    float bias = 0.4f;
    glm::mat4 lightViews[6], lightSpaceMatrices[6];

    virtual void push_pos();
    virtual void push_color();
    virtual void push_kc();
    virtual void push_kl();
    virtual void push_kq();
    virtual void push_far();
    virtual void push_bias();
    virtual void push_shadowMap();
    virtual void pushAll();

    virtual void updateMatrices();
    
    virtual void setLightPosSS();

    virtual void setShadowMatricesSS();

    virtual void configureDepthMap();

    virtual void begin();
		
	virtual void end();
};

struct PointLamp : public PointLight {
    Model *mptr;

    void setPos(const glm::vec3 &pos);
};

struct DirLamp : public DirLight {
    Model *mptr;

    void setPos(const glm::vec3 &pos);
};

} /* namespace algine */

#endif /* ALGINE_LIGHT_H */