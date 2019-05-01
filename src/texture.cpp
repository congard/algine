#ifndef ALGINE_TEXTURE_CPP
#define ALGINE_TEXTURE_CPP

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <GL/glew.h>
#include <iostream>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include "framebuffer.cpp"

namespace algine {
#define bindTexture2D(texture) glBindTexture(GL_TEXTURE_2D, texture)
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
 * `path` - full path to texture
 */
static GLuint loadTexture(const char *path, GLint internalformat = GL_RGB, GLuint format = GL_RGB, GLuint type = GL_UNSIGNED_BYTE) {
    GLuint texture = 0;
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
    } else std::cout << "Failed to load texture " << path << std::endl;

    stbi_image_free(data);

    return texture;
}

/**
 * Default params for bound texture
 */
void applyDefaultTexture2DParams() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

/**
 * Default params for specified texture
 */
void applyDefaultTexture2DParams(GLuint texture) {
    bindTexture2D(texture);
    applyDefaultTexture2DParams();
    bindTexture2D(0);   
}

/**
 * Default params for textures
 */
void applyDefaultTexture2DParams(GLuint *textures, size_t count) {
    for (size_t i = 0; i < count; i++) applyDefaultTexture2DParams(textures[i]);
}

inline size_t getTexComponentsCount(GLuint format) {
    switch (format) {
        case GL_RG:
            return 2;
        case GL_RGB:
        case GL_BGR:
            return 3;
        case GL_RGBA:
        case GL_BGRA:
            return 4;
        default:
            return 1;
    }
}

#define getTexImage(textureType, target, texture, width, height, format) \
    GLfloat* pixels = new GLfloat[width * height * getTexComponentsCount(format)]; \
    glActiveTexture(GL_TEXTURE0); \
    glBindTexture(textureType, texture); \
    glGetTexImage(target, 0, format, GL_FLOAT, pixels); \
    glBindTexture(textureType, 0); \

/**
 * Reads whole texture
 */
GLfloat* getTexImage2D(GLuint texture, size_t width, size_t height, GLuint format) {
    getTexImage(GL_TEXTURE_2D, GL_TEXTURE_2D, texture, width, height, format);
    return pixels;
}

/**
 * Reads whole texture
 * `target` - `GL_TEXTURE_CUBE_MAP_POSITIVE_X` and others
 */
GLfloat* getTexImageCube(GLenum target, GLuint texture, size_t width, size_t height, GLuint format) {
    getTexImage(GL_TEXTURE_CUBE_MAP, target, texture, width, height, format);
    return pixels;
}

#undef getTexImage

/**
 * Reads pixels from framebuffer
 */
inline GLfloat* getPixelsFB(GLuint framebuffer, size_t x, size_t y, size_t width, size_t height, GLuint format) {
    bindFramebuffer(framebuffer);
    GLfloat* pixels = new GLfloat(width * height * getTexComponentsCount(format));
    glReadPixels(x, y, width, height, format, GL_FLOAT, pixels);
    bindFramebuffer(0);
    return pixels;
}

/**
 * Reads pixels from texture
 */
GLfloat* getPixels(GLuint texture, size_t x, size_t y, size_t width, size_t height, GLuint format) {
    GLuint framebuffer;
    Framebuffer::create(&framebuffer);
    bindFramebuffer(framebuffer);
    Framebuffer::attachTexture2D(texture, COLOR_ATTACHMENT(0));
    GLfloat* pixels = getPixelsFB(framebuffer, x, y, width, height, format);
    Framebuffer::destroy(&framebuffer);
    return pixels;
}

void saveTexImage(const GLfloat *image, size_t width, size_t height, size_t inComponents, const std::string &path, size_t outComponents, bool flip = true) {
    unsigned char *data = new unsigned char[width * height * outComponents];
    for (size_t i = 0; i < width * height * outComponents; i++) data[i] = 0;

    size_t components = inComponents > outComponents ? outComponents : inComponents;

    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            for (size_t c = 0; c < components; c++) {
                data[(y * width + x) * outComponents + c] = 255 * image[(y * width + x) * inComponents + c];
            }
        }
    }
    
    stbi_flip_vertically_on_write(flip);
    stbi_write_bmp(path.c_str(), width, height, outComponents, data);

    delete[] data;
}

struct Texture {
    static void create(GLuint *id) {
        glGenTextures(1, id);
    }

    static void create(GLuint *id, GLsizei count) {
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
    GLuint startId, size, *texturesIds = nullptr;
    GLint *samplersLocations = nullptr;

	/* --- constructors, operators, destructor --- */

	TextureArray(GLuint startId, GLuint size) {
		this->startId = startId;
		this->size = size;
        samplersLocations = new GLint[size];
		texturesIds = new GLuint[size];
	}

	TextureArray() { /* empty for init or arrays */ }

	TextureArray(const TextureArray &src) {
		startId = src.startId;
		size = src.size;
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
		std::swap(size, other.size);
		std::swap(samplersLocations, other.samplersLocations);
		std::swap(texturesIds, other.texturesIds);
		std::swap(TEXTURE_TYPE, other.TEXTURE_TYPE);
	}

    void init(GLuint programId, std::string textureArray) {
		for (GLuint i = 0; i < size; i++) samplersLocations[i] = glGetUniformLocation(programId, (textureArray + "[" + std::to_string(i) + "]").c_str());
	}
	
	void bind(GLuint programId) {
		glUseProgram(programId);
		bind();
		glUseProgram(0);
	}
	
	void bind() {
		for (GLuint i = 0; i < size; i++) {
			glUniform1i(samplersLocations[i], startId + i);
			glActiveTexture(GL_TEXTURE0 + startId + i);
			glBindTexture(TEXTURE_TYPE, texturesIds[i]);
		}
	}
	
	void addTexture(GLuint index, GLuint textureId) {
		texturesIds[index] = textureId;
	}

    void fillTexIds(const GLuint defValue) {
        for (GLuint i = 0; i < size; i++) texturesIds[i] = defValue;
    }
};

} // namespace algine

#endif /* ALGINE_TEXTURE_CPP */
