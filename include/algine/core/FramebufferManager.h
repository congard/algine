#ifndef ALGINE_FRAMEBUFFERMANAGER_H
#define ALGINE_FRAMEBUFFERMANAGER_H

#include <algine/core/texture/Texture2DManager.h>
#include <algine/core/texture/TextureCubeManager.h>
#include <algine/core/RenderbufferManager.h>
#include <algine/core/OutputListManager.h>
#include <algine/core/FramebufferPtr.h>

#include <string>
#include <map>

namespace algine {
class FramebufferManager: public ManagerBase {
public:
    template<typename TMgr>
    class Attachments {
        friend class FramebufferManager;

    public:
        void set(const std::map<Attachment, TMgr> &managers);
        void setPaths(const std::map<Attachment, std::string> &paths);
        void setNames(const std::map<Attachment, std::string> &names);

        void add(const TMgr &manager, Attachment attachment);
        void addPath(const std::string &path, Attachment attachment);
        void addName(const std::string &name, Attachment attachment);

        const std::map<Attachment, TMgr>& get() const;
        const std::map<Attachment, std::string>& getPaths() const;
        const std::map<Attachment, std::string>& getNames() const;

    private:
        Attachments();

    private:
        std::map<Attachment, TMgr> m_managers;
        std::map<Attachment, std::string> m_paths, m_names;
    };

    using RenderbufferAttachments = Attachments<RenderbufferManager>;
    using Texture2DAttachments = Attachments<Texture2DManager>;
    using TextureCubeAttachments = Attachments<TextureCubeManager>;

    using OutputList = OutputListManager::Data;

public:
    void setOutputLists(const std::vector<OutputList> &lists);
    void setOutputLists(const std::vector<OutputListManager> &lists);
    void addOutputList(const OutputListManager &list);
    const std::vector<OutputListManager>& getOutputLists() const;

    RenderbufferAttachments& renderbufferAttachments();
    Texture2DAttachments& texture2DAttachments();
    TextureCubeAttachments& textureCubeAttachments();

    FramebufferPtr get();
    FramebufferPtr create();

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    RenderbufferAttachments m_renderbufferAttachments;
    Texture2DAttachments m_texture2DAttachments;
    TextureCubeAttachments m_textureCubeAttachments;
    std::vector<OutputListManager> m_outputLists;
};
}

#endif //ALGINE_FRAMEBUFFERMANAGER_H
