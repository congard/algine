#ifndef ALGINE_AMTLMANAGER_H
#define ALGINE_AMTLMANAGER_H

#include <algine/std/AMTLMaterialManager.h>

#include <vector>

namespace algine {
class AMTLManager: public Scriptable {
public:
    void setMaterials(const std::vector<AMTLMaterialManager> &materials);
    AMTLMaterialManager& addMaterial(const AMTLMaterialManager &material, const std::string &name = {});

    const std::vector<AMTLMaterialManager>& getMaterials();
    AMTLMaterialManager& getMaterial(std::string_view name);

    bool isMaterialExists(std::string_view name) const;

    void loadFile(std::string_view path, Lua *lua = nullptr);
    void loadScript(std::string_view script, Lua *lua = nullptr);

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;

private:
    void load(std::string_view s, bool path, Lua *lua = nullptr);

private:
    std::vector<AMTLMaterialManager> m_materials;
};
}

#endif //ALGINE_AMTLMANAGER_H
