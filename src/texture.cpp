#include <algine/texture.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <algine/framebuffer.h>
#include <iostream>
#include <algine/types.h>

using namespace std;

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

Texture::Texture() {
    glGenTextures(1, &id);
}

Texture::Texture(const uint target): Texture() {
    this->target = target;
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::bind() {
    glBindTexture(target, id);
}

void Texture::use(const uint slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(target, id);
}

void Texture::setParams(const std::map<uint, uint> &params) {
    for(const auto & key : params)
        glTexParameteri(target, key.first, key.second);
}

void Texture::setParams(const std::map<uint, float> &params) {
    for(const auto & key : params)
        glTexParameterf(target, key.first, key.second);
}

void Texture::setLOD(const uint _lod) {
    lod = _lod;
}

void Texture::setFormat(const uint _format) {
    format = _format;
}

void Texture::setWidth(const uint _width) {
    width = _width;
}

void Texture::setHeight(const uint _height) {
    height = _height;
}

void Texture::setWidthHeight(const uint _width, const uint _height) {
    width = _width;
    height = _height;
}

void Texture::update() {
    // last 3 params never used, but must be correct:
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    glTexImage2D(target, lod, format, width, height, 0, Red, GL_BYTE, nullptr);
}

void Texture::update(const uint dataFormat, const uint dataType, const void *const data) {
    glTexImage2D(target, lod, format, width, height, 0, dataFormat, dataType, data);
}

void Texture::unbind() {
    glBindTexture(target, 0);
}

uint Texture::getLOD() const {
    return lod;
}

uint Texture::getFormat() const {
    return format;
}

uint Texture::getWidth() const {
    return width;
}

uint Texture::getHeight() const {
    return height;
}

uint Texture::getId() const {
    return id;
}

void Texture::texFromFile(const std::string &path, uint _target, uint dataType, bool flipImage) {
    int channels;
    stbi_set_flip_vertically_on_load(flipImage);
    unsigned char *data = stbi_load(path.c_str(),
                                    reinterpret_cast<int *>(&width), reinterpret_cast<int *>(&height), &channels, 0);

    int formats[] = {Red, RG, RGB, RGBA};
    int dataFormat = formats[channels - 1];

    if (data) {
        glTexImage2D(_target, lod, format, width, height, 0, dataFormat, dataType, data);
        glGenerateMipmap(target);
    } else {
        std::cerr << "Failed to load texture " << path << std::endl;
        return;
    }

    stbi_image_free(data);
}

// TODO: remove
void Texture::create(uint *id) {
    glGenTextures(1, id);
}

void Texture::destroy(uint *id) {
    glDeleteTextures(1, id);
}

Texture2D::Texture2D(): Texture(GL_TEXTURE_2D) {}

void Texture2D::fromFile(const std::string &path, const uint dataType, const bool flipImage) {
    texFromFile(path, GL_TEXTURE_2D, dataType, flipImage);
}

map<uint, uint> Texture2D::defaultParams() {
    return map<uint, uint> {
        pair<uint, uint> {MinFilter, Linear},
        pair<uint, uint> {MagFilter, Linear},
        pair<uint, uint> {WrapU, ClampToEdge},
        pair<uint, uint> {WrapV, ClampToEdge}
    };
}

TextureCube::TextureCube(): Texture(GL_TEXTURE_CUBE_MAP) {}

void TextureCube::fromFile(const std::string &path, uint face, uint dataType, bool flipImage) {
    texFromFile(path, face, dataType, flipImage);
}

map<uint, uint> TextureCube::defaultParams() {
    return map<uint, uint> {
            pair<uint, uint> {MinFilter, Linear},
            pair<uint, uint> {MagFilter, Linear},
            pair<uint, uint> {WrapU, ClampToEdge},
            pair<uint, uint> {WrapV, ClampToEdge},
            pair<uint, uint> {WrapW, ClampToEdge}
    };
}

} // namespace algine