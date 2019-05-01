/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My github: https://github.com/congard
 * @author congard
 */

#ifndef ALGINE_LAMP_CPP
#define ALGINE_LAMP_CPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

#include <iostream>

#include "algine_renderer.cpp"
#include "model.cpp"

namespace algine {
struct Light {
    glm::vec3 pos, color;

	// constant term; usually kept at 1.0
    float kc;
    // linear term
    float kl;
    // quadratic term
    float kq;
    // far plane
    float far = 32.0f;
	    
    /* --- to access shader --- */
    GLint propLocations[6]; // one lamp has 6 properties
    
    #include <algine/sconstants.h>

    void loadLocations(GLint programId, GLuint id) {
        // uns - uniform name start
        std::string uns = ALGINE_NAME_CS_LAMPS + std::string("[") + std::to_string(id) + "].";
		propLocations[0] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_LAMP_POS).c_str());
		propLocations[1] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_LAMP_COLOR).c_str());
		propLocations[2] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_KC).c_str());
		propLocations[3] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_KL).c_str());
		propLocations[4] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_KQ).c_str());
        propLocations[5] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_FAR).c_str());
	}

    void push_lampPos() { setVec3(propLocations[0], pos); }

    void push_lampColor() { setVec3(propLocations[1], color); }

    void push_kc() { glUniform1f(propLocations[2], kc); }

    void push_kl() { glUniform1f(propLocations[3], kl); }

    void push_kq() { glUniform1f(propLocations[4], kq); }

    void push_far() { glUniform1f(propLocations[5], far); }

    void pushAll() {
        push_lampPos();
        push_lampColor();
        push_kc();
        push_kl();
        push_kq();
        push_far();
    }

    /* --- shadow mapping --- */
    static GLint shadowMatricesLocations[6], lightPosLocation;
    
    static void initShadowMatrices(GLuint programId) {
        for (size_t i = 0; i < 6; i++) shadowMatricesLocations[i] = glGetUniformLocation(programId, (std::string(ALGINE_NAME_SS_MAT_SHADOW) + "[" + std::to_string(i) + "]").c_str());
    }

    #include <algine/unsconstants.h>

    GLuint SHADOW_MAP_RESOLUTION;
    GLuint depthMapFBO, depthCubemap;
    glm::mat4 lightProjection, lightViews[6], lightSpaceMatrices[6];

    void initShadowMapping(GLuint resolution) {
        SHADOW_MAP_RESOLUTION = resolution;
        createDepthMap();
        createDepthMapFBO();
        updateMatrices();
        init();
    }

    void setLightPos() {
        setVec3(lightPosLocation, pos);
    }

    void setShadowMatrices() {
        for (size_t i = 0; i < 6; i++) setMat4(shadowMatricesLocations[i], lightSpaceMatrices[i]);
    }

    void updateMatrices() {
        lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, far);
        lightViews[0] = glm::lookAt(pos, glm::vec3(pos.x + 1, pos.y, pos.z), glm::vec3(0, -1, 0));
        lightViews[1] = glm::lookAt(pos, glm::vec3(pos.x - 1, pos.y, pos.z), glm::vec3(0, -1, 0));
        lightViews[2] = glm::lookAt(pos, glm::vec3(pos.x, pos.y + 1, pos.z), glm::vec3(0, 0, 1));
        lightViews[3] = glm::lookAt(pos, glm::vec3(pos.x, pos.y - 1, pos.z), glm::vec3(0, 0, -1));
        lightViews[4] = glm::lookAt(pos, glm::vec3(pos.x, pos.y, pos.z + 1), glm::vec3(0, -1, 0));
        lightViews[5] = glm::lookAt(pos, glm::vec3(pos.x, pos.y, pos.z - 1), glm::vec3(0, -1, 0));
        for (size_t i = 0; i < 6; i++) lightSpaceMatrices[i] = lightProjection * lightViews[i];
    }

    void createDepthMapFBO() {
        glGenFramebuffers(1, &depthMapFBO);
    }

    void createDepthMap() {
        glGenTextures(1, &depthCubemap);
    }

    void init() {
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		for (size_t i = 0; i < 6; i++) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void begin() {
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		glViewport(0, 0, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);
	}
		
	void end() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

    void swap(Light &other) {
        std::swap(pos, other.pos);
        std::swap(color, other.color);
        std::swap(kc, other.kc);
        std::swap(kl, other.kl);
        std::swap(kq, other.kq);
        std::swap(far, other.far);
        std::swap(propLocations, other.propLocations);
        std::swap(shadowMatricesLocations, other.shadowMatricesLocations);
        std::swap(SHADOW_MAP_RESOLUTION, other.SHADOW_MAP_RESOLUTION);
        std::swap(lightPosLocation, other.lightPosLocation);
        std::swap(depthMapFBO, other.depthMapFBO);
        std::swap(depthCubemap, other.depthCubemap);
        std::swap(lightProjection, other.lightProjection);
        std::swap(lightViews, other.lightViews);
        std::swap(lightSpaceMatrices, other.lightSpaceMatrices);
    }

    /* --- constructors, operators, destructor --- */

    Light() { /* empty for arrays */ }

    Light(const Light &src) {
        pos = src.pos;
        color = src.color;
        kc = src.kc;
        kl = src.kl;
        kq = src.kq;
        far = src.far;
        std::copy(src.propLocations, src.propLocations + 6, propLocations);
        std::copy(src.shadowMatricesLocations, src.shadowMatricesLocations + 6, shadowMatricesLocations);
        SHADOW_MAP_RESOLUTION = src.SHADOW_MAP_RESOLUTION;
        lightPosLocation = src.lightPosLocation;
        createDepthMapFBO();
        createDepthMap();
        lightProjection = src.lightProjection;
        std::copy(src.lightViews, src.lightViews + 6, lightViews);
        std::copy(src.lightSpaceMatrices, src.lightSpaceMatrices + 6, lightSpaceMatrices);

        #ifdef ALGINE_LOGGING
        std::cout << "Lamp copy constructor\n";
        #endif
    }

    Light(Light &&src) {
        src.swap(*this);
        #ifdef ALGINE_LOGGING
        std::cout << "Lamp move constructor\n";
        #endif
    }

    Light& operator = (const Light &rhs) {
        if (&rhs != this) Light(rhs).swap(*this);
        #ifdef ALGINE_LOGGING
        std::cout << "Lamp copy operator =\n";
        #endif

        return *this;
    }

    Light& operator = (Light &&rhs) {
        rhs.swap(*this);
        #ifdef ALGINE_LOGGING
        std::cout << "Lamp move operator =\n";
        #endif

        return *this;
    }

    ~Light() {
        glDeleteFramebuffers(1, &depthMapFBO);
        glDeleteTextures(1, &depthCubemap);
        #ifdef ALGINE_LOGGING
        std::cout << "~Lamp()\n";
        #endif
    }

    /* --- --- */
};

// define static variables
GLint Light::shadowMatricesLocations[6], Light::lightPosLocation;

struct Lamp : public Light {
    Model *mptr;

    void setPos(const glm::vec3 &pos) {
        Light::pos = pos;
        mptr->setPos(pos);
        mptr->transform();
    }
};

} // namespace algine

#endif /* ALGINE_LAMP_CPP */
