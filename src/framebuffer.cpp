#include <algine/framebuffer.h>
#include <GL/glew.h>

using namespace std;
using namespace tulz;

namespace algine {
#define attachmentsList m_attachmentsLists[m_activeList]

Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &m_id);

    m_attachmentsLists.emplace_back(std::vector<uint> {ColorAttachmentZero});
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &m_id);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

// TODO: not marked static because will be implemented "secure operations" check, which needs this pointer

void Framebuffer::attachTexture(const Texture2D *const texture, const uint attachment) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->getId(), 0);
}

void Framebuffer::attachTexture(const TextureCube *const texture, const uint attachment) {
    glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->getId(), 0);
}

void Framebuffer::attachRenderbuffer(const Renderbuffer *const renderbuffer, const uint attachment) {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer->getId());
}

void Framebuffer::addAttachmentsList(const std::vector<uint> &list) {
    m_attachmentsLists.emplace_back(list);
}

void Framebuffer::addOutput(const uint outIndex, const uint attachment) {
    if (outIndex == attachmentsList.size())
        attachmentsList.emplace_back(attachment);
    else if (outIndex > attachmentsList.size()) {
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
    glDrawBuffers(attachmentsList.size(), &attachmentsList[0]);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
            return pair<uint, uint> {Texture::RG, 2};
        case Texture::RGB:
        case Texture::RGB8:
        case Texture::RGB16F:
        case Texture::RGB32F:
            return pair<uint, uint> {Texture::RGB, 3};;
        case Texture::RGBA:
        case Texture::RGBA8:
        case Texture::RGBA16:
        case Texture::RGBA16F:
        case Texture::RGBA32F:
            return pair<uint, uint> {Texture::RGBA, 4};
        case Texture::DepthComponent:
            return pair<uint, uint> {Texture::DepthComponent, 1};
        default:
            return pair<uint, uint> {Texture::Red, 1};
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
        const uint width, const uint height, int format)
{
    if (format == -1) {
        glBindTexture(GL_TEXTURE_2D, getAttachedTexId(mode));
        format = getTex2DParam(GL_TEXTURE_INTERNAL_FORMAT);
        glBindTexture(GL_TEXTURE_2D, 0); // TODO: should be like Engine::defaultTexture2D->bind()
    }

    return algine::getPixels2D(mode, x, y, width, height, getTexFormatInfo(format));
}

PixelData Framebuffer::getAllPixels2D(const uint attachment, int format) {
    uint width, height;

    glBindTexture(GL_TEXTURE_2D, getAttachedTexId(attachment));
    if (format == -1)
        format = getTex2DParam(GL_TEXTURE_INTERNAL_FORMAT);
    width = getTex2DParam(GL_TEXTURE_WIDTH);
    height = getTex2DParam(GL_TEXTURE_HEIGHT);
    glBindTexture(GL_TEXTURE_2D, 0); // TODO: should be like Engine::defaultTexture2D->bind()

    return algine::getPixels2D(attachment, 0, 0, width, height, getTexFormatInfo(format));
}

PixelData Framebuffer::getAllPixelsCube(const uint face, const uint attachment, int format) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, getAttachedTexId(attachment));

    if (format == -1)
        format = getTexParam(face, GL_TEXTURE_INTERNAL_FORMAT);

    auto formatInfo = getTexFormatInfo(format);

    PixelData pixelData;
    pixelData.width = getTexParam(face, GL_TEXTURE_WIDTH);
    pixelData.height = getTexParam(face, GL_TEXTURE_HEIGHT);
    pixelData.componentsCount = formatInfo.second;
    pixelData.pixels = Array<float>(pixelData.width * pixelData.height * pixelData.componentsCount);

    glGetTexImage(face, 0, formatInfo.first, GL_FLOAT, pixelData.pixels.m_array);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // TODO: should be like Engine::defaultTextureCube->bind()

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

void Framebuffer::create(uint *id) {
    glGenFramebuffers(1, id);
}

void Framebuffer::attachTexture2D(const uint &textureId, const uint &colorAttachment) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, colorAttachment, GL_TEXTURE_2D, textureId, 0);
}

void Framebuffer::destroy(uint *id) {
    glDeleteFramebuffers(1, id);
}
}