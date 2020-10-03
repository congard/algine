#include <algine/core/Framebuffer.h>

#include <algine/core/Engine.h>

#include <GL/glew.h>

#define SOP_BOUND_PTR Engine::m_boundFramebuffer
#define SOP_OBJECT_TYPE SOPConstants::FramebufferObject
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME SOPConstants::FramebufferStr
#include "SOP.h"
#include "SOPConstants.h"

#include "texture/TexturePrivateTools.h"
#include "PublicObjectTools.h"

using namespace std;
using namespace tulz;

namespace algine {
vector<FramebufferPtr> Framebuffer::publicObjects;

Framebuffer::Framebuffer()
    : m_id(0),
      m_activeList(0),
      m_outputLists({{}})
{
    glGenFramebuffers(1, &m_id);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &m_id);
}

void Framebuffer::bind() const {
    commitBinding()
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void Framebuffer::unbind() const {
    checkBinding()
    commitUnbinding()
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::attachTexture(const Texture2DPtr &texture, Attachment attachment) {
    checkBinding()

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->getId(), 0);

    m_texture2DAttachments[attachment] = texture;
}

void Framebuffer::attachTexture(const TextureCubePtr &texture, Attachment attachment) {
    checkBinding()

    glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->getId(), 0);

    m_textureCubeAttachments[attachment] = texture;
}

void Framebuffer::attachRenderbuffer(const RenderbufferPtr &renderbuffer, Attachment attachment) {
    checkBinding()

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer->getId());

    m_renderbufferAttachments[attachment] = renderbuffer;
}

void Framebuffer::update() {
    checkBinding()

    const auto &list = m_outputLists[m_activeList];

    glDrawBuffers(list.size(), list.data());
}

void Framebuffer::clear(const uint buffersMask) {
    checkBinding()
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

void Framebuffer::setActiveOutputList(Index index) {
    m_activeList = index;
}

void Framebuffer::setOutputLists(const vector<OutputList> &lists) {
    m_outputLists = lists;
}

void Framebuffer::addOutputList(const OutputList &list) {
    m_outputLists.emplace_back(list);
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

// returns base format + components count
inline pair<uint, uint> getTexFormatInfo(const uint format) {
    switch (format) {
        case Texture::RG:
        case Texture::RG8:
        case Texture::RG16:
        case Texture::RG16F:
        case Texture::RG32F:
            return {Texture::RG, 2};
        case Texture::RGB:
        case Texture::RGB8:
        case Texture::RGB16F:
        case Texture::RGB32F:
            return {Texture::RGB, 3};
        case Texture::RGBA:
        case Texture::RGBA8:
        case Texture::RGBA16:
        case Texture::RGBA16F:
        case Texture::RGBA32F:
            return {Texture::RGBA, 4};
        case Texture::DepthComponent:
            return {Texture::DepthComponent, 1};
        default:
            return {Texture::Red, 1};
    }
}

inline PixelData getPixels2D(uint mode, uint x, uint y, uint width, uint height, const pair<uint, uint> &formatInfo)
{
    PixelData pixelData;
    pixelData.width = width;
    pixelData.height = height;
    pixelData.componentsCount = formatInfo.second;
    pixelData.pixels = Array<float>(width * height * pixelData.componentsCount);

    glReadBuffer(mode);
    glReadPixels(x, y, width, height, formatInfo.first, GL_FLOAT, pixelData.pixels.m_array);

    return pixelData;
}

PixelData Framebuffer::getPixels2D(uint mode, uint x, uint y, uint width, uint height, int format) const
{
    checkBinding()

    auto &texture = m_texture2DAttachments.at(mode);

    if (format == -1) {
        texture->bind();
        format = texture->getActualFormat();
        texture->unbind();
    }

    return algine::getPixels2D(mode, x, y, width, height, getTexFormatInfo(format));
}

PixelData Framebuffer::getAllPixels2D(uint attachment, int format) const {
    checkBinding()

    uint width, height;

    auto &texture = m_texture2DAttachments.at(attachment);

    texture->bind();

    if (format == -1)
        format = texture->getActualFormat();

    width = texture->getActualWidth();
    height = texture->getActualHeight();
    texture->unbind();

    return algine::getPixels2D(attachment, 0, 0, width, height, getTexFormatInfo(format));
}

PixelData Framebuffer::getAllPixelsCube(TextureCube::Face face, uint attachment, int format) const {
    checkBinding()

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

    glGetTexImage(static_cast<uint>(face), 0, formatInfo.first, GL_FLOAT, pixelData.pixels.m_array);

    texture->unbind();

    return pixelData;
}

uint Framebuffer::getId() const {
    return m_id;
}

bool Framebuffer::hasAttachment(Attachment attachment) {
    return getAttachedObjectType(attachment) != AttachedObjectType::None;
}

Framebuffer::AttachedObjectType Framebuffer::getAttachedObjectType(Attachment attachment) {
    auto isContains = [&](const auto &m)
    {
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

RenderbufferPtr& Framebuffer::getAttachedRenderbuffer(Attachment attachment) {
    return m_renderbufferAttachments[attachment];
}

Texture2DPtr& Framebuffer::getAttachedTexture2D(Attachment attachment) {
    return m_texture2DAttachments[attachment];
}

TextureCubePtr& Framebuffer::getAttachedTextureCube(Attachment attachment) {
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

FramebufferPtr Framebuffer::getByName(const string &name) {
    return PublicObjectTools::getByName<FramebufferPtr>(name);
}

Framebuffer* Framebuffer::byName(const string &name) {
    return PublicObjectTools::byName<Framebuffer>(name);
}
}