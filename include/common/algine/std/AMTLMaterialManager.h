#ifndef ALGINE_AMTLMATERIALMANAGER_H
#define ALGINE_AMTLMATERIALMANAGER_H

#include <algine/core/texture/Texture2DBuilder.h>
#include <algine/core/scene/GlobalScene.h>

#include <unordered_map>
#include <set>

namespace algine {
class AL_EXPORT AMTLMaterialManager: public Scriptable {
    friend class AMTLManager;

public:
    AMTLMaterialManager();
    explicit AMTLMaterialManager(std::string_view name);

    void setName(const std::string &name);
    void setFloat(const std::string &name, float value);
    void setTexture(const std::string &name, const Texture2DBuilder &texture);
    void setTextureName(const std::string &name, const std::string &texName);

    const std::string& getName() const;
    float getFloat(const std::string &name) const;
    bool hasFloat(const std::string &name) const;
    bool hasTexture(const std::string &name) const;

    const std::unordered_map<std::string, float>& getFloats() const;
    const std::unordered_map<std::string, Texture2DBuilder>& getTextures() const;
    const std::unordered_map<std::string, std::string>& getTextureNames() const;

    std::set<std::string> collectTextureNames() const;

    /**
     * Creates & returns texture of the specified name
     * @param texName
     * @param parent
     * @return pointer to a newly created texture if such name exists,
     * nullptr otherwise
     */
    Texture2D* loadTexture(const std::string &texName, Object *parent = GlobalScene::getInstance());

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;

private:
    std::string m_workingDirectory;
    std::string m_name;

    std::unordered_map<std::string, float> m_floats;
    std::unordered_map<std::string, Texture2DBuilder> m_textures;
    std::unordered_map<std::string, std::string> m_texNames;
};
}

#endif //ALGINE_AMTLMATERIALMANAGER_H
