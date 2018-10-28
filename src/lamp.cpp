/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
 * @author congard
 */

#ifndef ALGINE_LAMP_CPP
#define ALGINE_LAMP_CPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

#include <iostream>

#include "utils/shader_utils.cpp"
#include "model.cpp"

#define LAMP_FAR_PLANE 32.0f

class Lamp {
    public:
        glm::vec3 pos, color;

        // ambient strength param; recommended: 0.01
	    float ambientStrength;
	    // diffuse strength param; recommended: 1
	    float diffuseStrength;
	    // specular strength param; recommended: 0.1
	    float specularStrength;
	    // constant term; usually kept at 1.0
	    float kc;
	    // linear term
	    float kl;
	    // quadratic term
	    float kq;
	    // shininess; recommended: 32
	    int shininess;

        /* --- to access shader --- */
        GLuint propLocations[9]; // one lamp has 9 properties
        
        #include "sconstants.h"

        void loadLocations(GLuint programId, GLuint id) {
            // uns - uniform name start
            std::string uns = ALGINE_NAME_CS_LAMPS + std::string("[") + std::to_string(id) + "].";
            propLocations[0] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_AMBIENT_STRENGTH).c_str());
			propLocations[1] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_DIFFUSE_STRENGTH).c_str());
			propLocations[2] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_SPECULAR_STRENGTH).c_str());
			propLocations[3] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_SHININESS).c_str());
			propLocations[4] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_LAMP_POS).c_str());
			propLocations[5] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_LAMP_COLOR).c_str());
			propLocations[6] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_KC).c_str());
			propLocations[7] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_KL).c_str());
			propLocations[8] = glGetUniformLocation(programId, (uns + ALGINE_NAME_CS_LAMP_KQ).c_str());
		}

        #include "unsconstants.h"

        void push_ambientStrength() { glUniform1f(propLocations[0], ambientStrength); }

        void push_diffuseStrength() { glUniform1f(propLocations[1], diffuseStrength); }

        void push_specularStrength() { glUniform1f(propLocations[2], specularStrength); }

        void push_shininess() { glUniform1i(propLocations[3], shininess); }

        void push_lampPos() { su::setVec3(propLocations[4], pos); }

        void push_lampColor() { su::setVec3(propLocations[5], color); }

        void push_kc() { glUniform1f(propLocations[6], kc); }

        void push_kl() { glUniform1f(propLocations[7], kl); }

        void push_kq() { glUniform1f(propLocations[8], kq); }

        void pushAll() {
            push_ambientStrength();
            push_diffuseStrength();
            push_specularStrength();
            push_shininess();
            push_lampPos();
            push_lampColor();
            push_kc();
            push_kl();
            push_kq();
        }

        /* --- shadow mapping --- */
        static GLuint shadowMatricesLocations[6], lightPosLocation;
        
        static void initShadowMatrices(GLuint programId) {
            for (size_t i = 0; i < 6; i++) shadowMatricesLocations[i] = glGetUniformLocation(programId, ("shadowMatrices[" + std::to_string(i) + "]").c_str());
        }

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
            su::setVec3(lightPosLocation, pos);
        }

        void setShadowMatrices() {
            for (size_t i = 0; i < 6; i++) su::setMat4(shadowMatricesLocations[i], lightSpaceMatrices[i]);
        }

        void updateMatrices() {
            lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, LAMP_FAR_PLANE);
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

        void swap(Lamp &other) {
            std::swap(pos, other.pos);
            std::swap(color, other.color);
            std::swap(ambientStrength, other.ambientStrength);
            std::swap(diffuseStrength, other.diffuseStrength);
            std::swap(specularStrength, other.specularStrength);
            std::swap(kc, other.kc);
            std::swap(kl, other.kl);
            std::swap(kq, other.kq);
            std::swap(shininess, other.shininess);
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

        Lamp() { /* empty for arrays */ }

        Lamp(const Lamp &src) {
            pos = src.pos;
            color = src.color;
            ambientStrength = src.ambientStrength;
            diffuseStrength = src.diffuseStrength;
            specularStrength = src.specularStrength;
            kc = src.kc;
            kl = src.kl;
            kq = src.kq;
            shininess = src.shininess;
            std::copy(src.propLocations, src.propLocations + 9, propLocations);
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

        Lamp(Lamp &&src) {
            src.swap(*this);
            #ifdef ALGINE_LOGGING
            std::cout << "Lamp move constructor\n";
            #endif
        }

        Lamp& operator = (const Lamp &rhs) {
            if (&rhs != this) Lamp(rhs).swap(*this);
            #ifdef ALGINE_LOGGING
            std::cout << "Lamp copy operator =\n";
            #endif

            return *this;
        }

        Lamp& operator = (Lamp &&rhs) {
            rhs.swap(*this);
            #ifdef ALGINE_LOGGING
            std::cout << "Lamp move operator =\n";
            #endif

            return *this;
        }

        ~Lamp() {
            glDeleteFramebuffers(1, &depthMapFBO);
            glDeleteTextures(1, &depthCubemap);
            #ifdef ALGINE_LOGGING
            std::cout << "~Lamp()\n";
            #endif
        }

        /* --- --- */
};

// define static variables
GLuint Lamp::Lamp::shadowMatricesLocations[6], Lamp::lightPosLocation;

class LampModel : public Lamp {
    public:
        Model *mptr;

        void setPos(const glm::vec3 &pos) {
            Lamp::pos = pos;
            mptr->origin = pos;
        }
};

#endif /* ALGINE_LAMP_CPP */