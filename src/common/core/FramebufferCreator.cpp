#include <algine/core/FramebufferCreator.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/PtrMaker.h>

#include "internal/PublicObjectTools.h"

using namespace std;
using namespace algine::internal;

namespace algine {
void FramebufferCreator::setOutputLists(const vector<OutputList> &lists) {
    m_outputLists = lists;
}

void FramebufferCreator::addOutputList(const OutputList &list) {
    m_outputLists.emplace_back(list);
}

const vector<OutputList>& FramebufferCreator::getOutputLists() const {
    return m_outputLists;
}

FramebufferCreator::RenderbufferAttachments& FramebufferCreator::renderbufferAttachments() {
    return m_renderbufferAttachments;
}

FramebufferCreator::Texture2DAttachments& FramebufferCreator::texture2DAttachments() {
    return m_texture2DAttachments;
}

FramebufferCreator::TextureCubeAttachments& FramebufferCreator::textureCubeAttachments() {
    return m_textureCubeAttachments;
}

FramebufferPtr FramebufferCreator::get() {
    return PublicObjectTools::getPtr<FramebufferPtr>(this);
}

template<typename T>
struct type_holder {
    using type = T;
};

#define type_holder_get(holder) typename decltype(holder)::type

FramebufferPtr FramebufferCreator::create() {
    FramebufferPtr framebuffer = PtrMaker::make();
    framebuffer->setName(m_name);

    // init output lists
    framebuffer->removeOutputLists();

    for (const auto & list : m_outputLists)
        framebuffer->addOutputList(list);

    // attach objects
    auto attachAll = [&](auto &attachments, auto type) {
        auto attach = [&](const auto &ptr, Attachment attachment) {
            using Type = PtrMaker::PtrType<remove_reference_t<decltype(ptr)>>;

            if constexpr (is_same_v<Type, Renderbuffer>) {
                framebuffer->attachRenderbuffer(ptr, attachment);
            }

            if constexpr (is_same_v<Type, Texture2D> || is_same_v<Type, TextureCube>) {
                framebuffer->attachTexture(ptr, attachment);
            }
        };

        auto attachByName = [&](const string &name, Attachment attachment, auto objType) {
            using Type = type_holder_get(objType);

            // try to find object by name
            auto ptr = Type::getByName(name);

            if (ptr == nullptr)
                throw runtime_error("Public Object '" + name + "' does not exist");

            // attach object
            attach(ptr, attachment);
        };

        // attach using creators, paths and names

        using T = typename std::remove_reference_t<decltype(attachments)>::type;

        for (auto &p : attachments.value) {
            auto &v = p.second;

            if (auto name = std::get_if<Name>(&v); name) {
                attachByName(*name, p.first, type);
            } else if (auto creator = std::get_if<T>(&v); creator) {
                creator->setIOSystem(io());
                attach(creator->get(), p.first);
            }
        }
    };

    framebuffer->bind();

    attachAll(m_renderbufferAttachments, type_holder<Renderbuffer>());
    attachAll(m_texture2DAttachments, type_holder<Texture2D>());
    attachAll(m_textureCubeAttachments, type_holder<TextureCube>());

    framebuffer->update();
    framebuffer->unbind();

    PublicObjectTools::postCreateAccessOp("Framebuffer", this, framebuffer);

    return framebuffer;
}

void FramebufferCreator::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<FramebufferCreator>(s, path, lua, tenv);
}
}
