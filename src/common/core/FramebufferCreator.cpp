#include <algine/core/FramebufferCreator.h>

#include <algine/core/JsonHelper.h>
#include <algine/core/PtrMaker.h>

#include "FramebufferConfigTools.h"
#include "internal/PublicObjectTools.h"

using namespace std;
using namespace nlohmann;
using namespace algine::internal;

namespace algine {
namespace Config {
constant(Attachment, "attachment");
constant(Attachments, "attachments");
constant(OutputLists, "outputLists");
}

void FramebufferCreator::setOutputLists(const vector<OutputList> &lists) {
    m_outputLists = {};

    for (const auto & list : lists) {
        m_outputLists.emplace_back(list);
    }
}

void FramebufferCreator::setOutputLists(const vector<OutputListCreator> &lists) {
    m_outputLists = lists;
}

void FramebufferCreator::addOutputList(const OutputListCreator &list) {
    m_outputLists.emplace_back(list);
}

const vector<OutputListCreator>& FramebufferCreator::getOutputLists() const {
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

    for (const auto & list : m_outputLists) {
        framebuffer->addOutputList(list.create());
    }

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

        auto attachByPath = [&](const string &path, Attachment attachment, auto creatorType) {
            using TCreator = type_holder_get(creatorType);

            TCreator creator;
            creator.setIOSystem(io());
            creator.setWorkingDirectory(m_workingDirectory);
            creator.importFromFile(path);

            attach(creator.get(), attachment);
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

            if (auto path = std::get_if<Path>(&v); path) {
                attachByPath(path->str, p.first, type_holder<T>());
            } else if (auto name = std::get_if<Name>(&v); name) {
                attachByName(name->str, p.first, type);
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

template<typename T>
inline string typeName() {
    using TPlain = remove_reference_t<T>;

    if constexpr (is_same_v<TPlain, FramebufferCreator::RenderbufferAttachments>)
        return Config::Renderbuffer;

    if constexpr (is_same_v<TPlain, FramebufferCreator::Texture2DAttachments>)
        return Config::Texture2D;

    if constexpr (is_same_v<TPlain, FramebufferCreator::TextureCubeAttachments>)
        return Config::TextureCube;

    throw invalid_argument("Invalid template type");
}

void FramebufferCreator::import(const JsonHelper &jsonHelper) {
    const json &config = jsonHelper.json;

    auto importAttachments = [&](auto &obj) {
        const json &attachments = config[Config::Attachments];

        for (const auto & item : attachments) {
            if (item[Config::Type] == typeName<decltype(obj)>()) {
                auto attachment = Config::stringToAttachment(item[Config::Attachment]);

                if (item.contains(Config::Dump)) {
                    using TCreator = typename std::remove_reference_t<decltype(obj)>::type;

                    TCreator creator;
                    creator.setWorkingDirectory(m_workingDirectory);
                    creator.import(item[Config::Dump]);

                    obj.add(creator, attachment);
                } else if (item.contains(Config::Path)) {
                    obj.addPath(item[Config::Path], attachment);
                } else if (item.contains(Config::Name)) {
                    obj.addName(item[Config::Name], attachment);
                }
            }
        }
    };

    // load XXXAttachments
    if (config.contains(Config::Attachments)) {
        importAttachments(m_renderbufferAttachments);
        importAttachments(m_texture2DAttachments);
        importAttachments(m_textureCubeAttachments);
    }

    // load output lists
    if (config.contains(Config::OutputLists)) {
        for (const auto & list : config[Config::OutputLists]) {
            OutputListCreator outputListCreator;
            outputListCreator.import(list);
            m_outputLists.emplace_back(outputListCreator);
        }
    }

    Creator::import(jsonHelper);
}

JsonHelper FramebufferCreator::dump() {
    json config;

    // write attachments
    {
        // array with attachments of all types
        json attachments;

        // dump Attachments object and append it to the attachments json
        auto append = [&](auto &obj) {
            // array with attachments of obj type
            json attachmentsType;

            auto writeBlock = [&](const string &storeMethod, const json &data, uint attachment) {
                json block;
                block[Config::Type] = typeName<decltype(obj)>();
                block[Config::Attachment] = Config::attachmentToString(attachment);
                block[storeMethod] = data;
                attachmentsType.emplace_back(block);
            };

            // Creator type
            using T = typename std::remove_reference_t<decltype(obj)>::type;

            for (auto &p : obj.value) {
                auto &v = p.second;

                if (auto path = std::get_if<Path>(&v); path) {
                    writeBlock(Config::Path, path->str, p.first);
                } else if (auto name = std::get_if<Name>(&v); name) {
                    writeBlock(Config::Name, name->str, p.first);
                } else if (auto creator = std::get_if<T>(&v); creator) {
                    writeBlock(Config::Dump, creator->dump().json, p.first);
                }
            }

            // append to attachments
            if (!attachmentsType.empty()) {
                if (attachments.empty()) {
                    attachments = attachmentsType;
                } else {
                    attachments.insert(attachments.end(), attachmentsType.begin(), attachmentsType.end());
                }
            }
        };

        append(m_renderbufferAttachments);
        append(m_texture2DAttachments);
        append(m_textureCubeAttachments);

        if (!attachments.empty()) {
            config[Config::Attachments] = attachments;
        }
    }

    // write output lists
    for (auto & outputList : m_outputLists) {
        if (const auto &list = outputList.dump(); !list.empty()) {
            config[Config::OutputLists].emplace_back(list.json);
        }
    }

    JsonHelper result(config);
    result.append(Creator::dump());

    return result;
}
}
