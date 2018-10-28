/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
 * @author congard
 */

#ifndef ALGINE_TEXTURE_UTILS_CPP
#define ALGINE_TEXTURE_UTILS_CPP

#define STB_IMAGE_IMPLEMENTATION

#include "../lib/stb_image.h"
#include <GL/glew.h>
#include <iostream>

/**
 * Loading texture using stb_image.h
 * @param *path - path to texture
 * @return texture id
 */
static GLuint loadTexture(const char *path) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

	// flip texture
	stbi_set_flip_vertically_on_load(true);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else std::cout << "Failed to load texture" << std::endl;

    stbi_image_free(data);

    return texture;
}

static void bindADSTextures(GLuint ambientUnit, GLuint diffuseUnit, GLuint specularUnit,
	GLuint ambientTexture, GLuint diffuseTexture, GLuint specularTexture) {

	glUniform1i(ambientUnit, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ambientTexture);
		
	glUniform1i(diffuseUnit, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		
	glUniform1i(specularUnit, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, specularTexture);
}
	
static void bindADSNTextures(GLuint ambientUnit, GLuint diffuseUnit, GLuint specularUnit, GLuint normalUnit,
	GLuint ambientTexture, GLuint diffuseTexture, GLuint specularTexture, GLuint normalTexture) {

	bindADSTextures(ambientUnit, diffuseUnit, specularUnit, ambientTexture, diffuseTexture, specularTexture);
		
	glUniform1i(normalUnit, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normalTexture);
}

/**
 * To access sampler2D[SIZE]
 */
class TextureArray {
	private:
		GLuint startId, count, *samplersLocations = nullptr, *texturesIds = nullptr;

	public:
		GLuint TEXTURE_TYPE = GL_TEXTURE_CUBE_MAP;

		/* --- constructors, operators, destructor --- */

		TextureArray(GLuint startId, GLuint count) {
			this->startId = startId;
			this->count = count;
			samplersLocations = new GLuint[count];
			texturesIds = new GLuint[count];
		}

		TextureArray() { /* empty for init or arrays */ }

		TextureArray(const TextureArray &src) {
			startId = src.startId;
			count = src.count;
			samplersLocations = src.samplersLocations;
			texturesIds = src.texturesIds;
			TEXTURE_TYPE = src.TEXTURE_TYPE;
			#ifdef ALGINE_LOGGING
            std::cout << "TextureArray copy constructor\n";
            #endif
		}

		TextureArray(TextureArray &&src) {
            src.swap(*this);
            #ifdef ALGINE_LOGGING
            std::cout << "TextureArray move constructor\n";
            #endif
        }

        TextureArray& operator = (const TextureArray &rhs) {
            if (&rhs != this) TextureArray(rhs).swap(*this);
            #ifdef ALGINE_LOGGING
            std::cout << "TextureArray copy operator =\n";
            #endif

            return *this;
        }

        TextureArray& operator = (TextureArray &&rhs) {
            rhs.swap(*this);
            #ifdef ALGINE_LOGGING
            std::cout << "TextureArray move operator =\n";
            #endif

            return *this;
        }

		~TextureArray() {
			if (samplersLocations != nullptr) delete[] samplersLocations;
			if (texturesIds != nullptr) delete[] texturesIds;
			#ifdef ALGINE_LOGGING
            std::cout << "~TextureArray()\n";
            #endif
		}

		/* --- --- */

		void swap(TextureArray &other) {
			std::swap(startId, other.startId);
			std::swap(count, other.count);
			std::swap(samplersLocations, other.samplersLocations);
			std::swap(texturesIds, other.texturesIds);
			std::swap(TEXTURE_TYPE, other.TEXTURE_TYPE);
		}

		void init(GLuint programId, std::string textureArray) {
			for (GLuint i = 0; i < count; i++) samplersLocations[i] = glGetUniformLocation(programId, (textureArray + "[" + std::to_string(i) + "]").c_str());
		}
		
		void bind(GLuint programId) {
			glUseProgram(programId);
			bind();
			glUseProgram(0);
		}
		
		void bind() {
			for (GLuint i = 0; i < count; i++) {
				glUniform1i(samplersLocations[i], startId + i);
				glActiveTexture(GL_TEXTURE0 + startId + i);
				glBindTexture(TEXTURE_TYPE, texturesIds[i]);
			}
		}
		
		void addTexture(GLuint index, GLuint textureId) {
			texturesIds[index] = textureId;
		}
};

#endif /* ALGINE_TEXTURE_UTILS_CPP */