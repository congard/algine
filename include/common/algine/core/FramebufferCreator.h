#ifndef ALGINE_FRAMEBUFFERCREATOR_H
#define ALGINE_FRAMEBUFFERCREATOR_H

#include <algine/core/texture/Texture2DCreator.h>
#include <algine/core/texture/TextureCubeCreator.h>
#include <algine/core/RenderbufferCreator.h>
#include <algine/core/OutputList.h>
#include <algine/core/FramebufferPtr.h>

#include <variant>
#include <string>
#include <map>

namespace algine {
class FramebufferCreator: public Creator {
public:
    using Name = std::string;

    template<typename T>
    struct Attachments {
        using type = T;
        std::map<Attachment, std::variant<T, Name>> value;

        inline void addCreator(const T &p1, Attachment attachment) { value[attachment] = p1; }
        inline void addName(const std::string &p1, Attachment attachment) { value[attachment] = Name {p1}; }
    };

    using RenderbufferAttachments = Attachments<RenderbufferCreator>;
    using Texture2DAttachments = Attachments<Texture2DCreator>;
    using TextureCubeAttachments = Attachments<TextureCubeCreator>;

public:
    void setOutputLists(const std::vector<OutputList> &lists);
    void addOutputList(const OutputList &list);
    const std::vector<OutputList>& getOutputLists() const;

    RenderbufferAttachments& renderbufferAttachments();
    Texture2DAttachments& texture2DAttachments();
    TextureCubeAttachments& textureCubeAttachments();

    FramebufferPtr get();
    FramebufferPtr create();

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;

private:
    RenderbufferAttachments m_renderbufferAttachments;
    Texture2DAttachments m_texture2DAttachments;
    TextureCubeAttachments m_textureCubeAttachments;
    std::vector<OutputList> m_outputLists;
};
}

#endif //ALGINE_FRAMEBUFFERCREATOR_H
