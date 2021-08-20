#ifndef ALGINE_FRAMEBUFFERCREATOR_H
#define ALGINE_FRAMEBUFFERCREATOR_H

#include <algine/core/texture/Texture2DCreator.h>
#include <algine/core/texture/TextureCubeCreator.h>
#include <algine/core/RenderbufferCreator.h>
#include <algine/core/OutputListCreator.h>
#include <algine/core/FramebufferPtr.h>

#include <variant>
#include <string>
#include <map>

namespace algine {
class FramebufferCreator: public Creator {
public:
    struct Path { std::string str; };
    struct Name { std::string str; };

    template<typename T>
    struct Attachments {
        using type = T;
        std::map<Attachment, std::variant<T, Path, Name>> value;

        inline void add(const T &p1, Attachment attachment) { value[attachment] = p1; }
        inline void addPath(const std::string &p1, Attachment attachment) { value[attachment] = Path {p1}; }
        inline void addName(const std::string &p1, Attachment attachment) { value[attachment] = Name {p1}; }
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
