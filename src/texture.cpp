#ifndef ALGINE_TEXTURE_CPP
#define ALGINE_TEXTURE_CPP

#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <iostream>
#include "lib/stb_image.h"

namespace algine {
#define bindTexture(texture) glBindTexture(GL_TEXTURE_2D, texture)
#define cfgtex(texture, internalformat, format, width, height) glBindTexture(GL_TEXTURE_2D, texture); \
                                                               glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_FLOAT, NULL);
#define activeTexture(index, texture) glActiveTexture(index, texture)
    
#define COLOR_ATTACHMENT(n) (GL_COLOR_ATTACHMENT0 + n)
#define TEXTURE(n) (GL_TEXTURE0 + n)
// Activate & Bind
#define texture2DAB(index, texture) glActiveTexture(GL_TEXTURE0 + index); \
		                            glBindTexture(GL_TEXTURE_2D, texture);

/**
 * Loading texture using stb_image.h
 * @param *path - path to texture
 * @return texture id
 */
static GLuint loadTexture(const char *path, GLint internalformat = GL_RGB, GLuint format = GL_RGB, GLuint type = GL_UNSIGNED_BYTE) {
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
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else std::cout << "Failed to load texture" << std::endl;

    stbi_image_free(data);

    return texture;
}

void applyDefaultTextureParams() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

struct Texture {
    static void create(GLuint *id) {
        glGenTextures(1, id);
    }

    static void create(GLuint *id, GLuint count) {
        glGenTextures(count, id);
    }

    static void destroy(GLuint *id) {
        glDeleteTextures(1, id);
    }
};

/**
 * To access sampler2D[SIZE]
 */
struct TextureArray {
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

	private:
		GLuint startId, count, *samplersLocations = nullptr, *texturesIds = nullptr;
};

} // namespace algine

#endif /* ALGINE_TEXTURE_CPP */