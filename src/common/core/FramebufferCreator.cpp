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

template<typename TMgr>
void FramebufferCreator::Attachments<TMgr>::set(const map<Attachment, TMgr> &creators) {
    m_creators = creators;
}

template<typename TMgr>
void FramebufferCreator::Attachments<TMgr>::setPaths(const map<Attachment, string> &paths) {
    m_paths = paths;
}

template<typename TMgr>
void FramebufferCreator::Attachments<TMgr>::setNames(const map<Attachment, string> &names) {
    m_names = names;
}

template<typename TMgr>
void FramebufferCreator::Attachments<TMgr>::add(const TMgr &creator, Attachment attachment) {
    m_creators[attachment] = creator;
}

template<typename TMgr>
void FramebufferCreator::Attachments<TMgr>::addPath(const string &path, Attachment attachment) {
    m_paths[attachment] = path;
}

template<typename TMgr>
void FramebufferCreator::Attachments<TMgr>::addName(const string &name, Attachment attachment) {
    m_names[attachment] = name;
}

template<typename TMgr>
const map<Attachment, TMgr>& FramebufferCreator::Attachments<TMgr>::get() const {
    return m_creators;
}

template<typename TMgr>
const map<Attachment, string>& FramebufferCreator::Attachments<TMgr>::getPaths() const {
    return m_paths;
}

template<typename TMgr>
const map<Attachment, string>& FramebufferCreator::Attachments<TMgr>::getNames() const {
    return m_names;
}

template<typename TMgr>
FramebufferCreator::Attachments<TMgr>::Attachments() = default;

template class FramebufferCreator::Attachments<RenderbufferCreator>;
template class FramebufferCreator::Attachments<Texture2DCreator>;
template class FramebufferCreator::Attachments<TextureCubeCreator>;

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
    auto attachAll = [&](auto &attachments, auto type)
    {
        auto attach = [&](const auto &ptr, Attachment attachment)
        {
            using Type = PtrMaker::PtrType<remove_reference_t<decltype(ptr)>>;

            if constexpr (is_same_v<Type, Renderbuffer>) {
                framebuffer->attachRenderbuffer(ptr, attachment);
            }

            if constexpr (is_same_v<Type, Texture2D> || is_same_v<Type, TextureCube>) {
                framebuffer->attachTexture(ptr, attachment);
            }
        };

        auto attachByPath = [&](const string &path, Attachment attachment, auto creatorType)
        {
            using TCreator = type_holder_get(creatorType);

            TCreator creator;
            creator.setWorkingDirectory(m_workingDirectory);
            creator.importFromFile(path);

            attach(creator.get(), attachment);
        };

        auto attachByName = [&](const string &name, Attachment attachment, auto objType)
        {
            using Type = type_holder_get(objType);

            // try to find object by name
            auto ptr = Type::getByName(name);

            if (ptr == nullptr)
                throw runtime_error("Public Object '" + name + "' does not exist");

            // attach object
            attach(ptr, attachment);
        };

        // attach using creators, paths and names

        using T = typename decltype(attachments.m_creators)::mapped_type;

        for (auto & p : attachments.m_creators) {
            attach(p.second.get(), p.first);
        }

        for (const auto & p : attachments.m_paths) {
            attachByPath(p.second, p.first, type_holder<T>());
        }

        for (const auto & p : attachments.m_names) {
            attachByName(p.second, p.first, type);
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

    auto importAttachments = [&](auto &obj)
    {
        const json &attachments = config[Config::Attachments];

        for (const auto & item : attachments) {
            using namespace Config;

            if (item[Type] == typeName<decltype(obj)>()) {
                auto attachment = stringToAttachment(item[Config::Attachment]);

                if (item.contains(Dump)) {
                    using TCreator = typename decltype(obj.m_creators)::mapped_type;

                    TCreator creator;
                    creator.setWorkingDirectory(m_workingDirectory);
                    creator.import(item[Dump]);

                    obj.add(creator, attachment);
                } else if (item.contains(Path)) {
                    obj.addPath(item[Path], attachment);
                } else if (item.contains(Name)) {
                    obj.addName(item[Name], attachment);
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
        auto append = [&](auto &obj)
        {
            // array with attachments of obj type
            json attachmentsType;

            auto writeBlock = [&](const string &storeMethod, const json &data, uint attachment)
            {
                json block;
                block[Config::Type] = typeName<decltype(obj)>();
                block[Config::Attachment] = Config::attachmentToString(attachment);
                block[storeMethod] = data;
                attachmentsType.emplace_back(block);
            };

            for (auto &p : obj.m_creators)
                writeBlock(Config::Dump, p.second.dump().json, p.first);

            for (auto &p : obj.m_paths)
                writeBlock(Config::Path, p.second, p.first);

            for (auto &p : obj.m_names)
                writeBlock(Config::Name, p.second, p.first);

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
