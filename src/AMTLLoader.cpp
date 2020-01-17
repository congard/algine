#include <algine/AMTLLoader.h>
#include <algine/constants.h>
#include <nlohmann/json.hpp>
#include <tulz/Path>
#include <tulz/File>
#include <iostream>

using namespace std;
using namespace tulz;
using namespace algine::AlgineConstants;

namespace algine {
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

    cout << "name: " << type_str << "\n";
    auto textureJSONObject = materialJSONObject[type_str];

    for (auto &el : textureJSONObject.items()) {
        const string &key = el.key();
        cout << "key: " <<  key << "\n";

        if (key == AMTL::Texture::Path)
            materialTextureObject.path = el.value();
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