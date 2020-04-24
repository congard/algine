#include <algine/AMTLLoader.h>
#include <algine/texture/Texture.h>
#include <algine/constants/AMTL.h>

#include <nlohmann/json.hpp>
#include <tulz/Path>
#include <tulz/File>
#include <tulz/macros.h>
#include <iostream>

using namespace std;
using namespace tulz;
using namespace algine::constants;

namespace algine {
inline pair<uint, uint> getTexParam(const string &key, const string &value) {
    using namespace AMTL::Texture::Params;
    pair<uint, uint> param;

    switch_t(key)
    case_t(Keys::WrapU)
        param.first = Texture::WrapU;
    case_t(Keys::WrapV)
        param.first = Texture::WrapV;
    case_t(Keys::MinFilter)
        param.first = Texture::MinFilter;
    case_t(Keys::MagFilter)
        param.first = Texture::MagFilter;
    switch_t_end

    switch_t(value)
    case_t(Values::Repeat)
        param.second = Texture::Repeat;
    case_t(Values::ClampToEdge)
        param.second = Texture::ClampToEdge;
    case_t(Values::ClampToBorder)
        param.second = Texture::ClampToBorder;
    case_t(Values::MirroredRepeat)
        param.second = Texture::MirroredRepeat;
    case_t(Values::MirrorClampToEdge)
        param.second = Texture::MirrorClampToEdge;
    case_t(Values::Nearest)
        param.second = Texture::Nearest;
    case_t(Values::Linear)
        param.second = Texture::Linear;
    switch_t_end

    return param;
}

inline uint getSharedLevel(const string &str) {
    using namespace AMTL::Texture;

    switch_t(str)
    case_t(Unique)
        return AMTLLoader::Unique;
    case_t(ModelShared)
        return AMTLLoader::ModelShared;
    case_t(Shared)
        return AMTLLoader::Shared;
    switch_t_end

    cerr << "Unknown texture shared level \"" << str << "\"\n";
    return AMTLLoader::Shared;
}

inline void processTextureObject(const string &type_str, const nlohmann::json &materialJSONObject,
                                 AMTLLoader::MaterialObject &materialObject) {
    AMTLLoader::MaterialObject::TextureObject materialTextureObject = AMTLLoader::MaterialObject::TextureObject();
    uint type;

    switch_t(type_str)
    case_t(AMTL::Texture::Ambient)
        type = AMTLLoader::AmbientTexture;
    case_t(AMTL::Texture::Diffuse)
        type = AMTLLoader::DiffuseTexture;
    case_t(AMTL::Texture::Specular)
        type = AMTLLoader::SpecularTexture;
    case_t(AMTL::Texture::Normal)
        type = AMTLLoader::NormalTexture;
    case_t(AMTL::Texture::Reflection)
        type = AMTLLoader::ReflectionTexture;
    case_t(AMTL::Texture::Jitter)
        type = AMTLLoader::JitterTexture;
    default_t {
        cerr << "Error: unknown texture type " << type_str << "\n";
        return;
    }
    switch_t_end

    // reading texture block data
    auto textureJSONObject = materialJSONObject[type_str];
    for (auto &el : textureJSONObject.items()) {
        using namespace AMTL::Texture;

        switch_t(el.key())
        case_t(Path)
            materialTextureObject.path = el.value();
        case_t(SharedLevel)
            materialTextureObject.sharedLevel = getSharedLevel(el.value());
        case_t(Params::Params) {
            auto paramsObject = el.value();
            for (auto &pPair : paramsObject.items())
                materialTextureObject.params.insert(getTexParam(pPair.key(), pPair.value()));
        }
        switch_t_end
    }

    materialObject.textures[type] = materialTextureObject;
}

bool AMTLLoader::load(const std::string &path) {
    if (!Path(path).exists()) {
        std::cout << "AMTLLoader::load(): " << path << " not found\n";
        return false;
    }

    nlohmann::json json = nlohmann::json::parse(File(path, File::Read).readStr());

    for (auto materialJSONObject : json) {
        string name = materialJSONObject[AMTL::Name];
        MaterialObject materialObject = MaterialObject();

        for (auto &el : materialJSONObject.items()) {
            switch_t(el.key())
            case_t(AMTL::Name)
                continue;
            case_t(AMTL::AmbientStrength)
                materialObject.ambientStrength = el.value();
            case_t(AMTL::DiffuseStrength)
                materialObject.diffuseStrength = el.value();
            case_t(AMTL::SpecularStrength)
                materialObject.specularStrength = el.value();
            case_t(AMTL::Shininess)
                materialObject.shininess = el.value();
            case_t(AMTL::Jitter)
                materialObject.jitter = el.value();
            case_t(AMTL::Reflection)
                materialObject.reflection = el.value();
            default_t
                processTextureObject(el.key(), materialJSONObject, materialObject);
            switch_t_end
        }

        m_materials[name] = materialObject;
    }

    return true;
}
}