#ifndef ALGINE_MATERIAL_H
#define ALGINE_MATERIAL_H

#include <string>
#include <memory>
#include <algine/core/texture/Texture2DPtr.h>

namespace algine {
struct Material {
    std::string name;

    Texture2DPtr
        ambientTexture = nullptr,
        diffuseTexture = nullptr,
        specularTexture = nullptr,
        normalTexture = nullptr,
        reflectionTexture = nullptr,
        jitterTexture = nullptr;

    float
        ambientStrength = 0.01f,
        diffuseStrength = 1.0f,
        specularStrength = 1.0f,
        shininess = -1,
        reflection = 0.5f,
        jitter = 0.25f;
};
}

#endif /* ALGINE_MATERIAL_H */