#ifndef ALGINE_AMTLMATERIALMANAGER_H
#define ALGINE_AMTLMATERIALMANAGER_H

#include <algine/core/texture/Texture2DCreator.h>

#include <unordered_map>
#include <set>

namespace algine {
class AMTLMaterialManager: public Transferable, public IOProvider {
    friend class AMTLManager;

public:
    AMTLMaterialManager();
    explicit AMTLMaterialManager(std::string name);

    void setName(const std::string &name);
    void setFloat(const std::string &name, float value);
    void setTexture(const std::string &name, const Texture2DCreator &texture);
    void setTextureName(const std::string &name, const std::string &texName);
    void setTexturePath(const std::string &name, const std::string &path);

    const std::string& getName() const;
    float getFloat(const std::string &name) const;
    bool hasFloat(const std::string &name) const;
    bool hasTexture(const std::string &name) const;

    const std::unordered_map<std::string, float>& getFloats() const;
    const std::unordered_map<std::string, Texture2DCreator>& getTextures() const;
    const std::unordered_map<std::string, std::string>& getTextureNames() const;
    const std::unordered_map<std::string, std::string>& getTexturePaths() const;

    std::set<std::string> collectTextureNames() const;

    Texture2DPtr loadTexture(const std::string &texName);

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    std::string m_workingDirectory;
    std::string m_name;

    std::unordered_map<std::string, float> m_floats;
    std::unordered_map<std::string, Texture2DCreator> m_textures;
    std::unordered_map<std::string, std::string> m_texPaths, m_texNames;
};
}

#endif //ALGINE_AMTLMATERIALMANAGER_H
