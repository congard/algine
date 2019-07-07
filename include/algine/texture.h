#ifndef ALGINE_TEXTURE_H
#define ALGINE_TEXTURE_H

#include <algine/types.h>
#include <GL/glew.h>
#include <string>

// fill bound texture
#define _fillbtex(target, level, internalformat, format, width, height, type, data) \
    glTexImage2D(target, level, internalformat, width, height, 0, format, type, data)

#define _cfgtex(target, texture, level, internalformat, format, width, height, type) \
    glBindTexture(target, texture); \
    glTexImage2D(target, level, internalformat, width, height, 0, format, type, NULL);

#define COLOR_ATTACHMENT(n) (GL_COLOR_ATTACHMENT0 + n)
#define TEXTURE(n) (GL_TEXTURE0 + n)

namespace algine {

void activeTexture(const uint &index);

void bindTexture2D(const uint &texture);

// Activate & Bind 2D texture
void texture2DAB(const uint &index, const uint &texture);

void bindTextureCube(const uint &texture);

struct TextureParams {
    uint target; // specifies the target texture. Must be GL_TEXTURE_2D, etc
    int level = 0; // specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image
    int internalformat; // specifies the number of color components in the texture
    uint width, height;
    uint format; // specifies the format of the pixel data. The following symbolic values are accepted: GL_RED, GL_RG, GL_RGB, etc
    uint type = GL_FLOAT; // specifies the data type of the pixel data. The following symbolic values are accepted: GL_UNSIGNED_BYTE, GL_BYTE, etc
};

void cfgtex2D(const uint &texture, const int &internalformat, const uint &format, const uint &width, const uint &height);

void fillbtex(const TextureParams &tp, const void *data);

/**
 * Loading texture using stb_image.h
 * `path` - full path to texture
 */
uint loadTexture(const char *path, int internalformat = GL_RGB, uint format = GL_RGB, uint type = GL_UNSIGNED_BYTE);

/**
 * Default params for bound texture
 */
void applyDefaultTexture2DParams();

/**
 * Default params for bound texture
 */
void applyDefaultTextureCubeParams();

/**
 * Default params for specified texture
 */
void applyDefaultTexture2DParams(const uint &texture);

/**
 * Default params for textures
 */
void applyDefaultTexture2DParams(uint *textures, size_t count);

/**
 * Default params for specified cube texture
 */
void applyDefaultTextureCubeParams(const uint &texture);

size_t getTexComponentsCount(uint format);

/**
 * Reads whole texture
 */
float* getTexImage2D(uint texture, size_t width, size_t height, uint format);

/**
 * Reads whole texture
 * `target` - `GL_TEXTURE_CUBE_MAP_POSITIVE_X` and others
 */
float* getTexImageCube(GLenum target, GLuint texture, size_t width, size_t height, GLuint format);

/**
 * Reads pixels from framebuffer
 */
float* getPixelsFB(uint framebuffer, size_t x, size_t y, size_t width, size_t height, uint format);

/**
 * Reads pixels from texture
 */
float* getPixels(uint texture, size_t x, size_t y, size_t width, size_t height, uint format);

void saveTexImage(const float *image, size_t width, size_t height, size_t inComponents, const std::string &path, size_t outComponents, bool flip = true);

struct Texture {
    static void create(uint *id);

    static void create(uint *id, const uint &count);

    static void destroy(uint *id);

    static void destroy(uint *id, const uint &count);
};

struct TextureCube: public Texture {
    struct CubePaths {
        std::string right, left, top, bottom, back, front;

        CubePaths(
            const std::string &right, const std::string &left,
            const std::string &top, const std::string &bottom,
            const std::string &back, const std::string &front
        );

        CubePaths();
    };

    static void setFace(const TextureParams &params, const void *data);

    static void setFace(const TextureParams &params, const uint &texCube, const void *data);

    static void loadFaces(const TextureParams &_params, const CubePaths &paths);
};

}

#endif