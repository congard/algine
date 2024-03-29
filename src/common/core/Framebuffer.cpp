#include <algine/core/Framebuffer.h>
#include <algine/gl.h>

using namespace std;
using namespace tulz;

namespace algine {
AL_CONTEXT_OBJECT_DEFAULT_INITIALIZER(Framebuffer) {
    .obj = []() {
        auto framebuffer = new Framebuffer(std::false_type {});
        framebuffer->m_id = 0;
        return framebuffer;
    }()
};

Framebuffer::Framebuffer(Object *parent)
    : ContextObject(parent)
{
    glGenFramebuffers(1, &m_id);
}

Framebuffer::~Framebuffer() {
    if (this != getDefault()) {
        glDeleteFramebuffers(1, &m_id);
    }
}

void Framebuffer::bind() const {
    commitBinding();
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void Framebuffer::unbind() const {
    checkBinding();
    commitUnbinding();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachTexture(Texture2D *texture, Attachment attachment, bool takeOwnership) {
    checkBinding();

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->getId(), 0);

    if (takeOwnership)
        texture->setParent(this);

    m_texture2DAttachments[attachment] = texture;
}

void Framebuffer::attachTexture(TextureCube *texture, Attachment attachment, bool takeOwnership) {
    checkBinding();

    glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->getId(), 0);

    if (takeOwnership)
        texture->setParent(this);

    m_textureCubeAttachments[attachment] = texture;
}

void Framebuffer::attachRenderbuffer(Renderbuffer *renderbuffer, Attachment attachment, bool takeOwnership) {
    checkBinding();

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer->getId());

    if (takeOwnership)
        renderbuffer->setParent(this);

    m_renderbufferAttachments[attachment] = renderbuffer;
}

void Framebuffer::resizeAttachments(uint width, uint height) {
    auto resize = [&](const auto &m) {
        for (const auto & [attachment, obj] : m) {
            obj->bind();
            obj->setDimensions(width, height);
            obj->update();
        }
    };

    resize(m_renderbufferAttachments);
    resize(m_texture2DAttachments);
    resize(m_textureCubeAttachments);
}

void Framebuffer::setActiveOutputList(Index index) {
    m_activeList = index;
}

void Framebuffer::setOutputLists(const vector<OutputList> &lists) {
    m_outputLists = lists;
}

void Framebuffer::addOutputList(const OutputList &list) {
    m_outputLists.emplace_back(list);
}

void Framebuffer::removeOutputLists() {
    m_outputLists = {};
}

Index Framebuffer::getActiveOutputListIndex() const {
    return m_activeList;
}

OutputList& Framebuffer::getActiveOutputList() {
    return m_outputLists[m_activeList];
}

OutputList& Framebuffer::getLastOutputList() {
    return m_outputLists.back();
}

OutputList& Framebuffer::getOutputList(Index index) {
    return m_outputLists[index];
}

vector<OutputList>& Framebuffer::getOutputLists() {
    return m_outputLists;
}

void Framebuffer::update() {
    checkBinding();

    const auto &list = m_outputLists[m_activeList];

    glDrawBuffers(list.size(), list.data());
}

void Framebuffer::clear(const uint buffersMask) {
    checkBinding();
    glClear(buffersMask);
}

void Framebuffer::clearColorBuffer() {
    clear(ColorBuffer);
}

void Framebuffer::clearDepthBuffer() {
    clear(DepthBuffer);
}

void Framebuffer::clearStencilBuffer() {
    clear(StencilBuffer);
}

// returns base format + components count
inline pair<uint, uint> getTexFormatInfo(uint format) {
    switch (format) {
        case Texture::RG:
        case Texture::RG8:
        enable_if_desktop(case Texture::desktop_RG16:)
        case Texture::RG16F:
        case Texture::RG32F:
            return {Texture::RG, 2};
        case Texture::RGB:
        case Texture::RGB8:
        enable_if_desktop(case Texture::desktop_RGB16:)
        case Texture::RGB16F:
        case Texture::RGB32F:
            return {Texture::RGB, 3};
        case Texture::RGBA:
        case Texture::RGBA8:
        enable_if_desktop(case Texture::desktop_RGBA16:)
        case Texture::RGBA16F:
        case Texture::RGBA32F:
            return {Texture::RGBA, 4};
        case Texture::DepthComponent:
        case Texture::DepthComponent16:
        case Texture::DepthComponent24:
        case Texture::DepthComponent32F:
            return {Texture::DepthComponent, 1};
        case Texture::DepthStencil:
        case Texture::Depth24Stencil8:
        case Texture::Depth32FStencil8:
            return {Texture::DepthStencil, 2};
        default:
            return {Texture::Red, 1};
    }
}

void Framebuffer::readPixels(uint attachment,
    int x, int y, int width, int height,
    int format, DataType type, void *buffer) const
{
    checkBinding();
    glReadBuffer(attachment);
    glReadPixels(x, y, width, height, format, static_cast<GLenum>(type), buffer);
}

void Framebuffer::readPixels(uint attachment, int x, int y, int width, int height, void *buffer) const {
    auto &texture = m_texture2DAttachments.at(attachment);
    texture->bind();

    auto formatInfo = Texture::getFormatInfo(texture->getActualFormat());

    readPixels(attachment, x, y, width, height, formatInfo.baseFormat, formatInfo.dataType, buffer);
}

PixelData Framebuffer::getAllPixelsCube(TextureCube::Face face, uint attachment, int format) const {
    checkBinding();

    auto &texture = m_textureCubeAttachments.at(attachment);

    texture->bind();

    if (format == -1)
        format = texture->getActualFormat();

    auto formatInfo = getTexFormatInfo(format);

    PixelData pixelData;
    pixelData.width = texture->getActualWidth();
    pixelData.height = texture->getActualHeight();
    pixelData.componentsCount = formatInfo.second;
    pixelData.pixels = Array<float>(pixelData.width * pixelData.height * pixelData.componentsCount);

    // TODO: android
    enable_if_desktop(
        glGetTexImage(static_cast<uint>(face), 0, formatInfo.first, GL_FLOAT, pixelData.pixels.m_array)
    );

    texture->unbind();

    return pixelData;
}

bool Framebuffer::hasAttachment(Attachment attachment) {
    return getAttachedObjectType(attachment) != AttachedObjectType::None;
}

Framebuffer::AttachedObjectType Framebuffer::getAttachedObjectType(Attachment attachment) {
    auto isContains = [&](const auto &m) {
        return m.find(attachment) != m.end();
    };

    if (isContains(m_renderbufferAttachments))
        return AttachedObjectType::Renderbuffer;

    if (isContains(m_texture2DAttachments))
        return AttachedObjectType::Texture2D;

    if (isContains(m_textureCubeAttachments))
        return AttachedObjectType::TextureCube;

    return AttachedObjectType::None;
}

Renderbuffer* Framebuffer::getAttachedRenderbuffer(Attachment attachment) {
    return m_renderbufferAttachments[attachment];
}

Texture2D* Framebuffer::getAttachedTexture2D(Attachment attachment) {
    return m_texture2DAttachments[attachment];
}

TextureCube* Framebuffer::getAttachedTextureCube(Attachment attachment) {
    return m_textureCubeAttachments[attachment];
}

void Framebuffer::setClearColor(const float red, const float green, const float blue, const float alpha) {
    glClearColor(red, green, blue, alpha);
}

void Framebuffer::setClearDepth(const float depth) {
    glClearDepthf(depth);
}

void Framebuffer::setClearStencil(const int s) {
    glClearStencil(s);
}
}