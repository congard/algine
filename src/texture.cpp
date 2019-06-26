#ifndef ALGINE_TEXTURE_CPP
#define ALGINE_TEXTURE_CPP

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <GL/glew.h>
#include <iostream>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <algine/framebuffer.h>

// fill bound texture
#define _fillbtex(target, level, internalformat, format, width, height, type, data) \
    glTexImage2D(target, level, internalformat, width, height, 0, format, type, data)

#define _cfgtex(target, texture, level, internalformat, format, width, height, type) \
    glBindTexture(target, texture); \
    glTexImage2D(target, level, internalformat, width, height, 0, format, type, NULL);

#define COLOR_ATTACHMENT(n) (GL_COLOR_ATTACHMENT0 + n)
#define TEXTURE(n) (GL_TEXTURE0 + n)

namespace algine {
inline void activeTexture(const uint &index) {
    glActiveTexture(TEXTURE(index));
}

inline void bindTexture2D(const uint &texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
}

// Activate & Bind 2D texture
inline void texture2DAB(const uint &index, const uint &texture) {
    activeTexture(index);
	bindTexture2D(texture);
}

inline void bindTextureCube(const uint &texture) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}

struct TextureParams {
    uint target; // specifies the target texture. Must be GL_TEXTURE_2D, etc
    int level = 0; // specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image
    int internalformat; // specifies the number of color components in the texture
    uint width, height;
    uint format; // specifies the format of the pixel data. The following symbolic values are accepted: GL_RED, GL_RG, GL_RGB, etc
    uint type = GL_FLOAT; // specifies the data type of the pixel data. The following symbolic values are accepted: GL_UNSIGNED_BYTE, GL_BYTE, etc
};

inline void cfgtex2D(const uint &texture, const int &internalformat, const uint &format, const uint &width, const uint &height) {
    _cfgtex(GL_TEXTURE_2D, texture, 0, internalformat, format, width, height, GL_FLOAT);
}

inline void fillbtex(const TextureParams &tp, const void *data) {
    _fillbtex(tp.target, tp.level, tp.internalformat, tp.format, tp.width, tp.height, tp.type, data);
}

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
 * Default params for bound texture
 */
void applyDefaultTextureCubeParams() {
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

/**
 * Default params for specified texture
 */
void applyDefaultTexture2DParams(const GLuint &texture) {
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

/**
 * Default params for specified cube texture
 */
void applyDefaultTextureCubeParams(const GLuint &texture) {
    bindTextureCube(texture);
    applyDefaultTextureCubeParams();
    bindTextureCube(0);   
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

struct TextureCube: public Texture {
    struct CubePaths {
        std::string right, left, top, bottom, back, front;

        CubePaths(
            const std::string &right, const std::string &left,
            const std::string &top, const std::string &bottom,
            const std::string &back, const std::string &front
        ) {
            this->right = right;
            this->left = left;
            this->top = top;
            this->bottom = bottom;
            this->back = back;
            this->front = front;
        }

        CubePaths() {}
    };

    static void setFace(const TextureParams &params, const void *data) {
        fillbtex(params, data);
    }

    static void setFace(const TextureParams &params, const uint &texCube, const void *data) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texCube);
        fillbtex(params, data);
    }

    static void loadFaces(const TextureParams &_params, const CubePaths &paths) {
        TextureParams params = _params;

        int width, height, nrChannels;

        for (uint i = 0; i < 6; i++) {
            ubyte *data = stbi_load((*(&paths.right + i)).c_str(), &width, &height, &nrChannels, 0);

            params.width = width;
            params.height = height;
            params.target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;

            setFace(params, data);
            
            stbi_image_free(data);
        }
    }
};

} // namespace algine

#endif /* ALGINE_TEXTURE_CPP */
