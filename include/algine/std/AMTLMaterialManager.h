#ifndef ALGINE_AMTLMATERIALMANAGER_H
#define ALGINE_AMTLMATERIALMANAGER_H

#include <algine/core/texture/Texture2DManager.h>

#include <map>

namespace algine {
class AMTLMaterialManager: public Transferable {
    friend class AMTLManager;

public:
    enum class Texture {
        Ambient,
        Diffuse,
        Specular,
        Normal,
        Reflection,
        Jitter
    };

public:
    AMTLMaterialManager();
    explicit AMTLMaterialManager(const std::string &name);

    void setName(const std::string &name);

    void setAmbientStrength(float ambientStrength);
    void setDiffuseStrength(float diffuseStrength);
    void setSpecularStrength(float specularStrength);
    void setShininess(float shininess);
    void setReflection(float reflection);
    void setJitter(float jitter);

    void setTextures(const std::map<Texture, Texture2DManager> &textures);
    void setTextureNames(const std::map<Texture, std::string> &names);
    void setTexturePaths(const std::map<Texture, std::string> &paths);

    void setTexture(Texture type, const Texture2DManager &texture);
    void setTextureName(Texture type, const std::string &name);
    void setTexturePath(Texture type, const std::string &path);

    const std::string& getName() const;

    float getAmbientStrength() const;
    float getDiffuseStrength() const;
    float getSpecularStrength() const;
    float getShininess() const;
    float getReflection() const;
    float getJitter() const;

    const std::map<Texture, Texture2DManager>& getTextures() const;
    const std::map<Texture, std::string>& getTextureNames() const;
    const std::map<Texture, std::string>& getTexturePaths() const;

    Texture2DPtr loadTexture(Texture m);

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    std::string m_workingDirectory;
    std::string m_name;

    float m_ambientStrength, m_diffuseStrength, m_specularStrength;
    float m_reflection, m_jitter;
    float m_shininess;

    std::map<Texture, Texture2DManager> m_textures;
    std::map<Texture, std::string> m_texPaths, m_texNames;
};
}

#endif //ALGINE_AMTLMATERIALMANAGER_H
