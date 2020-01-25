#include <algine/AMTLLoader.h>
#include <algine/constants.h>
#include <algine/texture.h>
#include <nlohmann/json.hpp>
#include <tulz/Path>
#include <tulz/File>
#include <iostream>

using namespace std;
using namespace tulz;
using namespace algine::AlgineConstants;

namespace algine {
inline pair<uint, uint> getTexParam(const string &key, const string &value) {
    using namespace AMTL::Texture::Params;
    pair<uint, uint> param;

    if (key == Keys::WrapU)
        param.first = Texture::WrapU;
    else if (key == Keys::WrapV)
        param.first = Texture::WrapV;
    else if (key == Keys::MinFilter)
        param.first = Texture::MinFilter;
    else if (key == Keys::MagFilter)
        param.first = Texture::MagFilter;

    if (value == Values::Repeat)
        param.second = Texture::Repeat;
    else if (value == Values::ClampToEdge)
        param.second = Texture::ClampToEdge;
    else if (value == Values::ClampToBorder)
        param.second = Texture::ClampToBorder;
    else if (value == Values::MirroredRepeat)
        param.second = Texture::MirroredRepeat;
    else if (value == Values::MirrorClampToEdge)
        param.second = Texture::MirrorClampToEdge;
    else if (value == Values::Nearest)
        param.second = Texture::Nearest;
    else if (value == Values::Linear)
        param.second = Texture::Linear;

    return param;
}

inline void processTextureObject(const string &type_str, const nlohmann::json &materialJSONObject,
                                 AMTLLoader::MaterialObject &materialObject) {
    AMTLLoader::MaterialObject::TextureObject materialTextureObject = AMTLLoader::MaterialObject::TextureObject();
    uint type;

    if (type_str == AMTL::Texture::Ambient)
        type = AMTLLoader::AmbientTexture;
    else if (type_str == AMTL::Texture::Diffuse)
        type = AMTLLoader::DiffuseTexture;
    else if (type_str == AMTL::Texture::Specular)
        type = AMTLLoader::SpecularTexture;
    else if (type_str == AMTL::Texture::Normal)
        type = AMTLLoader::NormalTexture;
    else if (type_str == AMTL::Texture::Reflection)
        type = AMTLLoader::ReflectionTexture;
    else if (type_str == AMTL::Texture::Jitter)
        type = AMTLLoader::JitterTexture;
    else {
        cerr << "Error: unknown texture type " << type_str << "\n";
        return;
    }

    // reading texture block data
    auto textureJSONObject = materialJSONObject[type_str];
    for (auto &el : textureJSONObject.items()) {
        using namespace AMTL::Texture;

        const string &key = el.key();
        if (key == Path)
            materialTextureObject.path = el.value();
        else if (key == Params::Params) {
            auto paramsObject = el.value();
            for (auto &pPair : paramsObject.items())
                materialTextureObject.params.insert(getTexParam(pPair.key(), pPair.value()));
        }
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
            if (el.key() == AMTL::Name)
                continue;
            else if (el.key() == AMTL::AmbientStrength)
                materialObject.ambientStrength = el.value();
            else if (el.key() == AMTL::DiffuseStrength)
                materialObject.diffuseStrength = el.value();
            else if (el.key() == AMTL::SpecularStrength)
                materialObject.specularStrength = el.value();
            else if (el.key() == AMTL::Shininess)
                materialObject.shininess = el.value();
            else if (el.key() == AMTL::Jitter)
                materialObject.jitter = el.value();
            else if (el.key() == AMTL::Reflection)
                materialObject.reflection = el.value();
            else
                processTextureObject(el.key(), materialJSONObject, materialObject);
        }

        m_materials[name] = materialObject;
    }

    return true;
}
}