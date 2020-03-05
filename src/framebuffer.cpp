#include <algine/framebuffer.h>
#include <GL/glew.h>

using namespace std;

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