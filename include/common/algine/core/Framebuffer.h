#ifndef ALGINE_FRAMEBUFFER_H
#define ALGINE_FRAMEBUFFER_H

#include <algine/core/texture/Texture2D.h>
#include <algine/core/texture/TextureCube.h>
#include <algine/core/texture/PixelData.h>
#include <algine/core/FramebufferPtr.h>
#include <algine/core/Renderbuffer.h>
#include <algine/core/OutputList.h>

#include <algine/templates.h>
#include <algine/types.h>

#include <vector>

namespace algine {
class Framebuffer: public Object {
    friend class Engine;

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
    Framebuffer();
    ~Framebuffer();

    void bind() const;
    void unbind() const;

    void attachTexture(const Texture2DPtr &texture, Attachment attachment);
    void attachTexture(const TextureCubePtr &texture, Attachment attachment);
    void attachRenderbuffer(const RenderbufferPtr &renderbuffer, Attachment attachment);

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

    PixelData getPixels2D(uint mode, uint x, uint y, uint width, uint height, int format = -1) const;
    PixelData getAllPixels2D(uint attachment, int format = -1) const;
    PixelData getAllPixelsCube(TextureCube::Face face, uint attachment, int format = -1) const;

    uint getId() const;

    bool hasAttachment(Attachment attachment);
    AttachedObjectType getAttachedObjectType(Attachment attachment);
    RenderbufferPtr& getAttachedRenderbuffer(Attachment attachment);
    Texture2DPtr& getAttachedTexture2D(Attachment attachment);
    TextureCubePtr& getAttachedTextureCube(Attachment attachment);

    static void setClearColor(float red, float green, float blue, float alpha = 1.0f);
    static void setClearDepth(float depth);
    static void setClearStencil(int s);

public:
    static FramebufferPtr getByName(const std::string &name);
    static Framebuffer* byName(const std::string &name);

public:
    static std::vector<FramebufferPtr> publicObjects;

private:
    uint m_id;
    uint m_activeList;
    std::vector<OutputList> m_outputLists;
    std::map<Attachment, RenderbufferPtr> m_renderbufferAttachments;
    std::map<Attachment, Texture2DPtr> m_texture2DAttachments;
    std::map<Attachment, TextureCubePtr> m_textureCubeAttachments;
};
}

#endif /* ALGINE_FRAMEBUFFER_H */