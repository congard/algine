#ifndef ALGINE_FRAMEBUFFERBUILDER_H
#define ALGINE_FRAMEBUFFERBUILDER_H

#include <algine/core/texture/Texture2DBuilder.h>
#include <algine/core/texture/TextureCubeBuilder.h>
#include <algine/core/RenderbufferBuilder.h>
#include <algine/core/OutputList.h>
#include <algine/core/Framebuffer.h>

#include <variant>
#include <string>
#include <map>

namespace algine {
class AL_EXPORT FramebufferBuilder: public Builder {
    AL_BUILDER(Framebuffer)

public:
    using Name = std::string;

    template<typename T>
    struct Attachments {
        using type = T;
        std::map<Attachment, std::variant<T, Name>> value;

        inline void addBuilder(const T &p1, Attachment attachment) { value[attachment] = p1; }
        inline void addName(const std::string &p1, Attachment attachment) { value[attachment] = Name {p1}; }
    };

    using RenderbufferAttachments = Attachments<RenderbufferBuilder>;
    using Texture2DAttachments = Attachments<Texture2DBuilder>;
    using TextureCubeAttachments = Attachments<TextureCubeBuilder>;

public:
    void setOutputLists(const std::vector<OutputList> &lists);
    void addOutputList(const OutputList &list);
    const std::vector<OutputList>& getOutputLists() const;

    RenderbufferAttachments& renderbufferAttachments();
    Texture2DAttachments& texture2DAttachments();
    TextureCubeAttachments& textureCubeAttachments();

protected:
    Object* createImpl() override;
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;

private:
    RenderbufferAttachments m_renderbufferAttachments;
    Texture2DAttachments m_texture2DAttachments;
    TextureCubeAttachments m_textureCubeAttachments;
    std::vector<OutputList> m_outputLists;
};
}

#endif //ALGINE_FRAMEBUFFERBUILDER_H
