#ifndef ALGINE_FRAMEBUFFERCREATOR_H
#define ALGINE_FRAMEBUFFERCREATOR_H

#include <algine/core/texture/Texture2DCreator.h>
#include <algine/core/texture/TextureCubeCreator.h>
#include <algine/core/RenderbufferCreator.h>
#include <algine/core/OutputListCreator.h>
#include <algine/core/FramebufferPtr.h>

#include <string>
#include <map>

namespace algine {
class FramebufferCreator: public Creator {
public:
    template<typename T>
    class Attachments {
        friend class FramebufferCreator;

    public:
        void set(const std::map<Attachment, T> &creators);
        void setPaths(const std::map<Attachment, std::string> &paths);
        void setNames(const std::map<Attachment, std::string> &names);

        void add(const T &creator, Attachment attachment);
        void addPath(const std::string &path, Attachment attachment);
        void addName(const std::string &name, Attachment attachment);

        const std::map<Attachment, T>& get() const;
        const std::map<Attachment, std::string>& getPaths() const;
        const std::map<Attachment, std::string>& getNames() const;

    private:
        Attachments();

    private:
        std::map<Attachment, T> m_creators;
        std::map<Attachment, std::string> m_paths, m_names;
    };

    using RenderbufferAttachments = Attachments<RenderbufferCreator>;
    using Texture2DAttachments = Attachments<Texture2DCreator>;
    using TextureCubeAttachments = Attachments<TextureCubeCreator>;

    using OutputList = OutputListCreator::Data;

public:
    void setOutputLists(const std::vector<OutputList> &lists);
    void setOutputLists(const std::vector<OutputListCreator> &lists);
    void addOutputList(const OutputListCreator &list);
    const std::vector<OutputListCreator>& getOutputLists() const;

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
    std::vector<OutputListCreator> m_outputLists;
};
}

#endif //ALGINE_FRAMEBUFFERCREATOR_H
