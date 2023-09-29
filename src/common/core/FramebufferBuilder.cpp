#include <algine/core/FramebufferBuilder.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/PtrMaker.h>

namespace algine {
void FramebufferBuilder::setOutputLists(const std::vector<OutputList> &lists) {
    m_outputLists = lists;
}

void FramebufferBuilder::addOutputList(const OutputList &list) {
    m_outputLists.emplace_back(list);
}

const std::vector<OutputList>& FramebufferBuilder::getOutputLists() const {
    return m_outputLists;
}

FramebufferBuilder::RenderbufferAttachments& FramebufferBuilder::renderbufferAttachments() {
    return m_renderbufferAttachments;
}

FramebufferBuilder::Texture2DAttachments& FramebufferBuilder::texture2DAttachments() {
    return m_texture2DAttachments;
}

FramebufferBuilder::TextureCubeAttachments& FramebufferBuilder::textureCubeAttachments() {
    return m_textureCubeAttachments;
}

Object* FramebufferBuilder::createImpl() {
    auto framebuffer = new Framebuffer(getActualParent());
    framebuffer->setName(m_name);

    // init output lists
    framebuffer->removeOutputLists();

    for (const auto & list : m_outputLists)
        framebuffer->addOutputList(list);

    // attach objects
    auto attachAll = [&]<typename B>(Attachments<B> &attachments) {
        using T = B::result_t;

        auto attach = [&](T *ptr, Attachment attachment) {
            if constexpr (std::is_same_v<T, Renderbuffer>) {
                framebuffer->attachRenderbuffer(ptr, attachment);
            }

            if constexpr (std::is_same_v<T, Texture2D> || std::is_same_v<T, TextureCube>) {
                framebuffer->attachTexture(ptr, attachment);
            }
        };

        auto attachByName = [&](const std::string &name, Attachment attachment) {
            // try to find an object by name
            auto ptr = getByName<T*>(name);

            if (ptr == nullptr)
                throw std::runtime_error(Object::getTypeName<T>() + " '" + name + "' does not exist");

            // attach object
            attach(ptr, attachment);
        };

        // attach using builders, paths and names

        for (auto &p : attachments.value) {
            auto &v = p.second;

            if (auto name = std::get_if<Name>(&v); name) {
                attachByName(*name, p.first);
            } else if (auto builder = std::get_if<B>(&v); builder) {
                builder->setIOSystem(io());
                attach(builder->get(), p.first);
            }
        }
    };

    framebuffer->bind();

    attachAll(m_renderbufferAttachments);
    attachAll(m_texture2DAttachments);
    attachAll(m_textureCubeAttachments);

    framebuffer->update();
    framebuffer->unbind();

    return framebuffer;
}

void FramebufferBuilder::exec(const std::string &s, bool path, Lua *lua, sol::environment *tenv) {
    exec_t<FramebufferBuilder>(s, path, lua, tenv);
}
}
