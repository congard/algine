#include <algine/core/FramebufferBuilder.h>
#include <algine/core/Framebuffer.h>
#include <algine/core/PtrMaker.h>

using namespace std;

namespace algine {
void FramebufferBuilder::setOutputLists(const vector<OutputList> &lists) {
    m_outputLists = lists;
}

void FramebufferBuilder::addOutputList(const OutputList &list) {
    m_outputLists.emplace_back(list);
}

const vector<OutputList>& FramebufferBuilder::getOutputLists() const {
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

template<typename T>
struct type_holder {
    using type = T;
};

#define type_holder_get(holder) typename decltype(holder)::type

Object* FramebufferBuilder::createImpl() {
    auto framebuffer = new Framebuffer(getActualParent());
    framebuffer->setName(m_name);

    // init output lists
    framebuffer->removeOutputLists();

    for (const auto & list : m_outputLists)
        framebuffer->addOutputList(list);

    // attach objects; C++20 lambda template
    auto attachAll = [&](auto &attachments, auto type) {
        auto attach = [&](auto ptr, Attachment attachment) {
            using Type = remove_pointer_t<decltype(ptr)>;

            if constexpr (is_same_v<Type, Renderbuffer>) {
                framebuffer->attachRenderbuffer(ptr, attachment);
            }

            if constexpr (is_same_v<Type, Texture2D> || is_same_v<Type, TextureCube>) {
                framebuffer->attachTexture(ptr, attachment);
            }
        };

        auto attachByName = [&](const string &name, Attachment attachment, auto objType) {
            using Type = type_holder_get(objType);

            // try to find an object by name
            auto ptr = getByName<Type*>(name);

            if (ptr == nullptr)
                throw runtime_error(Object::getTypeName<Type>() + " '" + name + "' does not exist");

            // attach object
            attach(ptr, attachment);
        };

        // attach using builders, paths and names

        using T = typename std::remove_reference_t<decltype(attachments)>::type;

        for (auto &p : attachments.value) {
            auto &v = p.second;

            if (auto name = std::get_if<Name>(&v); name) {
                attachByName(*name, p.first, type);
            } else if (auto builder = std::get_if<T>(&v); builder) {
                builder->setIOSystem(io());
                attach(builder->get(), p.first);
            }
        }
    };

    framebuffer->bind();

    attachAll(m_renderbufferAttachments, type_holder<Renderbuffer>());
    attachAll(m_texture2DAttachments, type_holder<Texture2D>());
    attachAll(m_textureCubeAttachments, type_holder<TextureCube>());

    framebuffer->update();
    framebuffer->unbind();

    return framebuffer;
}

void FramebufferBuilder::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<FramebufferBuilder>(s, path, lua, tenv);
}
}
