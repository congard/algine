#ifndef ALGINE_FRAMEBUFFER_H
#define ALGINE_FRAMEBUFFER_H

#include <algine/core/ContextObject.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/texture/TextureCube.h>
#include <algine/core/texture/PixelData.h>
#include <algine/core/Renderbuffer.h>
#include <algine/core/OutputList.h>

#include <algine/templates.h>
#include <algine/types.h>

#include <vector>

namespace algine {
AL_CONTEXT_OBJECT(Framebuffer) {
    AL_CONTEXT_OBJECT_IMPL(Framebuffer)

public:
    enum AttachmentType {
        ColorAttachmentZero = GL_COLOR_ATTACHMENT0,
        DepthAttachment = GL_DEPTH_ATTACHMENT,
        StencilAttachment = GL_STENCIL_ATTACHMENT,
        DepthStencilAttachment = GL_DEPTH_STENCIL_ATTACHMENT,
        EmptyAttachment = GL_NONE
    };

    enum BufferMask {
        ColorBuffer = 0x00004000,
        DepthBuffer = 0x00000100,
        StencilBuffer = 0x00000400
    };

    enum class AttachedObjectType {
        None,
        Renderbuffer,
        Texture2D,
        TextureCube
    };

public:
    explicit Framebuffer(Object *parent = defaultParent());
    ~Framebuffer() override;

    void bind() const;
    void unbind() const;

    void attachTexture(Texture2D *texture, Attachment attachment, bool takeOwnership = true);
    void attachTexture(TextureCube *texture, Attachment attachment, bool takeOwnership = true);
    void attachRenderbuffer(Renderbuffer *renderbuffer, Attachment attachment, bool takeOwnership = true);

    void resizeAttachments(uint width, uint height);

    void setActiveOutputList(Index index);
    void setOutputLists(const std::vector<OutputList> &lists);
    void addOutputList(const OutputList &list = {});
    void removeOutputLists();

    Index getActiveOutputListIndex() const;
    OutputList& getActiveOutputList();
    OutputList& getLastOutputList();
    OutputList& getOutputList(Index index);
    std::vector<OutputList>& getOutputLists();

    void update();
    void clear(uint buffersMask);
    void clearColorBuffer();
    void clearDepthBuffer();
    void clearStencilBuffer();

    void readPixels(uint attachment, int x, int y, int width, int height, int format, DataType type, void *buffer) const;
    void readPixels(uint attachment, int x, int y, int width, int height, void *buffer) const;

    /// #solgen #ignore
    PixelData getAllPixelsCube(TextureCube::Face face, uint attachment, int format = -1) const;

    bool hasAttachment(Attachment attachment);
    AttachedObjectType getAttachedObjectType(Attachment attachment);
    Renderbuffer* getAttachedRenderbuffer(Attachment attachment);
    Texture2D* getAttachedTexture2D(Attachment attachment);
    TextureCube* getAttachedTextureCube(Attachment attachment);

    static void setClearColor(float red, float green, float blue, float alpha = 1.0f);
    static void setClearDepth(float depth);
    static void setClearStencil(int s);

private:
    uint m_activeList {0};
    std::vector<OutputList> m_outputLists {{}};
    std::map<Attachment, Renderbuffer*> m_renderbufferAttachments;
    std::map<Attachment, Texture2D*> m_texture2DAttachments;
    std::map<Attachment, TextureCube*> m_textureCubeAttachments;
};
}

#endif /* ALGINE_FRAMEBUFFER_H */