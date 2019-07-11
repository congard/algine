#ifndef ALGINE_MATERIAL_H
#define ALGINE_MATERIAL_H

#include <string>
#include <vector>
#include <algine/types.h>

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

    uint
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

    void recycle();
};

typedef Material AlgineMT;

// Algine Material Lib
struct AlgineMTL {
    std::vector<AlgineMT> materials;

    bool load(const std::string &path);

    int forName(const std::string &name);
};

}

#endif /* ALGINE_MATERIAL_H */