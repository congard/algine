#ifndef ALGINE_AMTLLOADER_H
#define ALGINE_AMTLLOADER_H

#include <string>
#include <map>
#include <algine/types.h>

namespace algine {
class AMTLLoader {
public:
    struct MaterialObject {
        struct TextureObject {
            std::string path;
            std::map<uint, uint> params;
        };

        std::map<uint, TextureObject> textures; // type, TextureObject

        float
            ambientStrength = 0.01f,
            diffuseStrength = 1.0f,
            specularStrength = 1.0f,
            shininess = -1,
            reflection = 0.5f,
            jitter = 0.25f;
    };

public:
    enum TextureTypes {
        AmbientTexture,
        DiffuseTexture,
        SpecularTexture,
        NormalTexture,
        ReflectionTexture,
        JitterTexture
    };

    bool load(const std::string &path);

public:
    std::map<std::string, MaterialObject> m_materials; // name, MaterialObject
};
}

#endif //ALGINE_AMTLLOADER_H
