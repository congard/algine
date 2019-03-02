#ifndef ALGINE_MATERIAL_CPP
#define ALGINE_MATERIAL_CPP

#include <vector>
#include <string>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "lib/json.hpp"
#include "io.cpp"
#include "constants.h"

namespace algine {
struct Material {
    std::string
        name,
        texAmbientPath,
        texDiffusePath,
        texSpecularPath,
        texNormalPath,
        texReflectionPath,
        texJitterPath;

    GLuint
        ambientTexture = 0,
        diffuseTexture = 0,
        specularTexture = 0,
        normalTexture = 0,
        reflectionTexture = 0,
        jitterTexture = 0;

    float
        ambientStrength = 0.01f,
        diffuseStrength = 1.0f,
        specularStrength = 1.0f,
        shininess = -1,
        reflection = 0.5f,
        jitter = 0.25f;

    void recycle() {
        glDeleteTextures(1, &ambientTexture);
        glDeleteTextures(1, &diffuseTexture);
        glDeleteTextures(1, &specularTexture);
        glDeleteTextures(1, &normalTexture);
        glDeleteTextures(1, &reflectionTexture);
        glDeleteTextures(1, &jitterTexture);
    }
};

typedef Material AlgineMT;

// Algine Material Lib
struct AlgineMTL {
    std::vector<AlgineMT> materials;

    bool load(const std::string &path) {
        if (!io::exists(path.c_str())) {
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
        nlohmann::json j = nlohmann::json::parse(io::read(path));

        for (size_t i = 0; i < j.size(); i++) {
            // sie - set if exists
            #define sie(key, var) \
                if (j[i].find(key) != j[i].end()) var = j[i][key]
            
            AlgineMT amt;
            amt.name = j[i][ALGINE_MATERIAL_NAME];

            sie(ALGINE_MATERIAL_AMBIENT_TEX, amt.texAmbientPath);
            sie(ALGINE_MATERIAL_DIFFUSE_TEX, amt.texDiffusePath);
            sie(ALGINE_MATERIAL_SPECULAR_TEX, amt.texSpecularPath);
            sie(ALGINE_MATERIAL_NORMAL_TEX, amt.texNormalPath);
            sie(ALGINE_MATERIAL_REFLECTION_TEX, amt.texReflectionPath);
            sie(ALGINE_MATERIAL_JITTER_TEX, amt.texJitterPath);

            sie(ALGINE_MATERIAL_REFLECTION, amt.reflection);
            sie(ALGINE_MATERIAL_JITTER, amt.jitter);

            sie(ALGINE_MATERIAL_AMBIENT_STRENGTH, amt.ambientStrength);
            sie(ALGINE_MATERIAL_DIFFUSE_STRENGTH, amt.diffuseStrength);
            sie(ALGINE_MATERIAL_SPECULAR_STRENGTH, amt.specularStrength);
            sie(ALGINE_MATERIAL_SHININESS, amt.shininess);

            materials.push_back(amt);

            #undef sie
        }

        return true;
    }

    int forName(const std::string &name) {
        for (size_t i = 0; i < materials.size(); i++) if (materials[i].name == name) return i;
        return -1;
    }
};

}

#endif /* ALGINE_MATERIAL_CPP */