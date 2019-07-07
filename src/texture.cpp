#include <algine/texture.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <algine/framebuffer.h>
#include <iostream>
#include <algine/types.h>

namespace algine {
void activeTexture(const uint &index) {
    glActiveTexture(TEXTURE(index));
}

void bindTexture2D(const uint &texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
}

// Activate & Bind 2D texture
void texture2DAB(const uint &index, const uint &texture) {
    activeTexture(index);
	bindTexture2D(texture);
}

void bindTextureCube(const uint &texture) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}

void cfgtex2D(const uint &texture, const int &internalformat, const uint &format, const uint &width, const uint &height) {
    _cfgtex(GL_TEXTURE_2D, texture, 0, internalformat, format, width, height, GL_FLOAT);
}

void fillbtex(const TextureParams &tp, const void *data) {
    _fillbtex(tp.target, tp.level, tp.internalformat, tp.format, tp.width, tp.height, tp.type, data);
}

/**
 * Loading texture using stb_image.h
 * `path` - full path to texture
 */
uint loadTexture(const char *path, int internalformat, uint format, uint type) {
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
void applyDefaultTexture2DParams(const uint &texture) {
    bindTexture2D(texture);
    applyDefaultTexture2DParams();
    bindTexture2D(0);   
}

/**
 * Default params for textures
 */
void applyDefaultTexture2DParams(uint *textures, size_t count) {
    for (size_t i = 0; i < count; i++) applyDefaultTexture2DParams(textures[i]);
}

/**
 * Default params for specified cube texture
 */
void applyDefaultTextureCubeParams(const uint &texture) {
    bindTextureCube(texture);
    applyDefaultTextureCubeParams();
    bindTextureCube(0);   
}

size_t getTexComponentsCount(uint format) {
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
float* getTexImage2D(uint texture, size_t width, size_t height, uint format) {
    getTexImage(GL_TEXTURE_2D, GL_TEXTURE_2D, texture, width, height, format);
    return pixels;
}

/**
 * Reads whole texture
 * `target` - `GL_TEXTURE_CUBE_MAP_POSITIVE_X` and others
 */
float* getTexImageCube(GLenum target, GLuint texture, size_t width, size_t height, GLuint format) {
    getTexImage(GL_TEXTURE_CUBE_MAP, target, texture, width, height, format);
    return pixels;
}

#undef getTexImage

/**
 * Reads pixels from framebuffer
 */
float* getPixelsFB(uint framebuffer, size_t x, size_t y, size_t width, size_t height, uint format) {
    bindFramebuffer(framebuffer);
    GLfloat* pixels = new GLfloat(width * height * getTexComponentsCount(format));
    glReadPixels(x, y, width, height, format, GL_FLOAT, pixels);
    bindFramebuffer(0);
    return pixels;
}

/**
 * Reads pixels from texture
 */
float* getPixels(uint texture, size_t x, size_t y, size_t width, size_t height, uint format) {
    GLuint framebuffer;
    Framebuffer::create(&framebuffer);
    bindFramebuffer(framebuffer);
    Framebuffer::attachTexture2D(texture, COLOR_ATTACHMENT(0));
    GLfloat* pixels = getPixelsFB(framebuffer, x, y, width, height, format);
    Framebuffer::destroy(&framebuffer);
    return pixels;
}

void saveTexImage(const float *image, size_t width, size_t height, size_t inComponents, const std::string &path, size_t outComponents, bool flip) {
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

// struct Texture
void Texture::create(uint *id) {
    glGenTextures(1, id);
}

void Texture::create(uint *id, const uint &count) {
    glGenTextures(count, id);
}

void Texture::destroy(uint *id) {
    glDeleteTextures(1, id);
}

void Texture::destroy(uint *id, const uint &count) {
    glDeleteTextures(count, id);
}

// struct TextureCube::CubePaths
TextureCube::CubePaths::CubePaths(
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

TextureCube::CubePaths::CubePaths() {}

// struct TextureCube
void TextureCube::setFace(const TextureParams &params, const void *data) {
    fillbtex(params, data);
}

void TextureCube::setFace(const TextureParams &params, const uint &texCube, const void *data) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, texCube);
    fillbtex(params, data);
}

void TextureCube::loadFaces(const TextureParams &_params, const CubePaths &paths) {
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

} // namespace algine