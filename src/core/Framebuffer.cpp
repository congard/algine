#include <algine/core/Framebuffer.h>

#include <algine/core/Engine.h>

#include <GL/glew.h>

#define SOP_BOUND_PTR Engine::m_boundFramebuffer
#define SOP_OBJECT_TYPE SOPConstants::FramebufferObject
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME SOPConstants::FramebufferStr
#include "SOP.h"
#include "SOPConstants.h"

using namespace std;
using namespace tulz;

namespace algine {
#define attachmentsList m_attachmentsLists[m_activeList]

Framebuffer::Framebuffer()
    : m_id(0),
      m_activeList(0),
      m_attachmentsLists({{ColorAttachmentZero}})
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

void Framebuffer::attachTexture(const Texture2D *const texture, const uint attachment) {
    checkBinding()
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->getId(), 0);
}

void Framebuffer::attachTexture(const TextureCube *const texture, const uint attachment) {
    checkBinding()
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->getId(), 0);
}

void Framebuffer::attachRenderbuffer(const Renderbuffer *const renderbuffer, const uint attachment) {
    checkBinding()
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer->getId());
}

void Framebuffer::addAttachmentsList(const std::vector<uint> &list) {
    m_attachmentsLists.emplace_back(list);
}

void Framebuffer::addOutput(const uint outIndex, const uint attachment) {
    if (outIndex == attachmentsList.size()) {
        attachmentsList.emplace_back(attachment);
    } else if (outIndex > attachmentsList.size()) {
        vector<uint> tmp(outIndex + 1, EmptyAttachment);
        for (uint i = 0; i < attachmentsList.size(); ++i)
            tmp[i] = attachmentsList[i];
        tmp[outIndex] = attachment;
        attachmentsList = tmp;
    } else {
        attachmentsList[outIndex] = attachment;
    }
}

inline void inlineOptimizeAttachmentsList(vector<uint> &attachments) {
    for (int i = static_cast<int>(attachments.size()) - 1; i >= 0; --i) {
        if (attachments[i] != Framebuffer::EmptyAttachment) {
            attachments.erase(attachments.begin() + i + 1, attachments.end());
            return;
        }
    }

    attachments.clear();
}

void Framebuffer::removeOutput(const uint outIndex, const bool optimizeList) {
    if (outIndex < attachmentsList.size()) {
        attachmentsList[outIndex] = EmptyAttachment;

        if (optimizeList && outIndex == attachmentsList.size() - 1)
            inlineOptimizeAttachmentsList(attachmentsList);
    } else {
        throw runtime_error(
                "Incorrect outIndex value. Active list: " +
                to_string(m_activeList) +
                "\nAttachments: " +
                to_string(attachmentsList.size()) +
                "\nGiven outIndex: " + to_string(outIndex));
    }
}

void Framebuffer::optimizeAttachmentsList() {
    inlineOptimizeAttachmentsList(attachmentsList);
}

void Framebuffer::update() {
    checkBinding()
    glDrawBuffers(attachmentsList.size(), &attachmentsList[0]);
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

void Framebuffer::setActiveAttachmentsList(const uint index) {
    m_activeList = index;
}

void Framebuffer::setAttachmentsList(const uint index, const vector<uint> &list) {
    m_attachmentsLists[index] = list;
}

void Framebuffer::setAttachmentsLists(const vector<vector<uint>> &lists) {
    m_attachmentsLists = lists;
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
            return {Texture::RGB, 3};;
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

inline uint getTexParam(const uint target, const uint param) {
    constexpr uint mipLevel = 0;
    int p;
    glGetTexLevelParameteriv(target, mipLevel, param, &p);
    return p;
}

inline uint getTex2DParam(const uint param) {
    return getTexParam(GL_TEXTURE_2D, param);
}

inline uint getAttachedTexId(const uint attachment) {
    int id;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &id);
    return id;
}

// TODO: SOP: bindDefaultX

inline PixelData getPixels2D(const uint mode, const uint x, const uint y,
        const uint width, const uint height, const pair<uint, uint> &formatInfo)
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

PixelData Framebuffer::getPixels2D(const uint mode, const uint x, const uint y,
        const uint width, const uint height, int format) const
{
    checkBinding()

    if (format == -1) {
        glBindTexture(GL_TEXTURE_2D, getAttachedTexId(mode));
        format = getTex2DParam(GL_TEXTURE_INTERNAL_FORMAT);
        Engine::defaultTexture2D()->bind();
    }

    return algine::getPixels2D(mode, x, y, width, height, getTexFormatInfo(format));
}

PixelData Framebuffer::getAllPixels2D(const uint attachment, int format) const {
    checkBinding()

    uint width, height;

    glBindTexture(GL_TEXTURE_2D, getAttachedTexId(attachment));

    if (format == -1)
        format = getTex2DParam(GL_TEXTURE_INTERNAL_FORMAT);

    width = getTex2DParam(GL_TEXTURE_WIDTH);
    height = getTex2DParam(GL_TEXTURE_HEIGHT);
    Engine::defaultTexture2D()->bind();

    return algine::getPixels2D(attachment, 0, 0, width, height, getTexFormatInfo(format));
}

PixelData Framebuffer::getAllPixelsCube(const TextureCube::Face face, const uint attachment, int format) const {
    checkBinding()
    glBindTexture(GL_TEXTURE_CUBE_MAP, getAttachedTexId(attachment));

    uint faceVal = static_cast<uint>(face);

    if (format == -1)
        format = getTexParam(faceVal, GL_TEXTURE_INTERNAL_FORMAT);

    auto formatInfo = getTexFormatInfo(format);

    PixelData pixelData;
    pixelData.width = getTexParam(faceVal, GL_TEXTURE_WIDTH);
    pixelData.height = getTexParam(faceVal, GL_TEXTURE_HEIGHT);
    pixelData.componentsCount = formatInfo.second;
    pixelData.pixels = Array<float>(pixelData.width * pixelData.height * pixelData.componentsCount);

    glGetTexImage(faceVal, 0, formatInfo.first, GL_FLOAT, pixelData.pixels.m_array);
    Engine::defaultTextureCube()->bind();

    return pixelData;
}

uint Framebuffer::getActiveAttachmentsList() const {
    return m_activeList;
}

std::vector<uint> Framebuffer::getAttachmentsList(const uint index) const {
    return m_attachmentsLists[index];
}

vector<vector<uint>> Framebuffer::getAttachmentsLists() const {
    return m_attachmentsLists;
}

uint Framebuffer::getId() const {
    return m_id;
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