#include <algine/material.h>

#include <iostream>
#include <GL/glew.h>
#include <nlohmann/json.hpp>
#include <algine/constants.h>
#include <tulz/Path>
#include <tulz/File>

using namespace tulz;

namespace algine {
void Material::recycle() {
    glDeleteTextures(1, &ambientTexture);
    glDeleteTextures(1, &diffuseTexture);
    glDeleteTextures(1, &specularTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &reflectionTexture);
    glDeleteTextures(1, &jitterTexture);
}

bool AlgineMTL::load(const std::string &path) {
    if (!Path(path).exists()) {
        std::cout << "AlgineMTL::load: " << path << " not found\n";
        return false;
    }
    /**
     * [
     *      {
     *          "name":"material name",
     *          -----------------------
     *          "reflectionTex":"/path/to/reflection/texture",
     *          "jitterTex":"jitter/texture"
     *          -----------------------
     *          "reflection":0.0,
     *          "jitter":1.0
     *      }
     * ]
     **/
    using namespace AlgineConstants::AMTL;

    nlohmann::json j = nlohmann::json::parse(File(path, File::ReadText).readStr());

    for (size_t i = 0; i < j.size(); i++) {
        // sie - set if exists
        #define sie(key, var) \
            if (j[i].find(key) != j[i].end()) var = j[i][key]

        AlgineMT amt;
        amt.name = j[i][Name];

        sie(AmbientTex, amt.texAmbientPath);
        sie(DiffuseTex, amt.texDiffusePath);
        sie(SpecularTex, amt.texSpecularPath);
        sie(NormalTex, amt.texNormalPath);
        sie(ReflectionTex, amt.texReflectionPath);
        sie(JitterTex, amt.texJitterPath);

        sie(Reflection, amt.reflection);
        sie(Jitter, amt.jitter);

        sie(AmbientStrength, amt.ambientStrength);
        sie(DiffuseStrength, amt.diffuseStrength);
        sie(SpecularStrength, amt.specularStrength);
        sie(Shininess, amt.shininess);

        materials.push_back(amt);

        #undef sie
    }

    return true;
}

int AlgineMTL::forName(const std::string &name) {
    for (size_t i = 0; i < materials.size(); i++) if (materials[i].name == name) return i;
    return -1;
}

}