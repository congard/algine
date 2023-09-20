#ifndef ALGINE_MATERIAL_H
#define ALGINE_MATERIAL_H

#include <algine/platform.h>

#include <unordered_map>
#include <string>

namespace algine {
class Texture2D;

class AL_EXPORT Material {
public:
    constexpr static auto AmbientTexture = "ambient";
    constexpr static auto DiffuseTexture = "diffuse";
    constexpr static auto SpecularTexture = "specular";
    constexpr static auto NormalTexture = "normal";
    constexpr static auto ReflectionTexture = "reflection";
    constexpr static auto JitterTexture = "jitter";

    constexpr static auto AmbientStrength = "ambientStrength";
    constexpr static auto DiffuseStrength = "diffuseStrength";
    constexpr static auto SpecularStrength = "specularStrength";
    constexpr static auto Shininess = "shininess";
    constexpr static auto Reflection = "reflection";
    constexpr static auto Jitter = "jitter";

public:
    void setName(const std::string &name);
    const std::string& getName() const;

    void setFloat(const std::string &name, float value);
    void setTexture2D(const std::string &name, Texture2D *value);

    float getFloat(const std::string &name) const;
    float getFloat(const std::string &name, float defaultValue) const;
    Texture2D* getTexture2D(const std::string &name) const;
    Texture2D* getTexture2D(const std::string &name, Texture2D *defaultValue) const;

    void setFloats(const std::unordered_map<std::string, float> &floats);
    void setTextures2D(const std::unordered_map<std::string, Texture2D*> &textures);

    const std::unordered_map<std::string, float>& getFloats() const;
    const std::unordered_map<std::string, Texture2D*>& getTextures2D() const;

    bool hasFloat(const std::string &name) const;
    bool hasTexture2D(const std::string &name) const;

private:
    std::string m_materialName;
    std::unordered_map<std::string, float> m_floats;
    std::unordered_map<std::string, Texture2D*> m_textures2D;
};
}

#endif /* ALGINE_MATERIAL_H */