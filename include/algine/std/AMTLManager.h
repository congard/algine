#ifndef ALGINE_AMTLMANAGER_H
#define ALGINE_AMTLMANAGER_H

#include <algine/std/AMTLMaterialManager.h>

#include <vector>

namespace algine {
class AMTLManager: public Transferable {
public:
    void setMaterials(const std::vector<AMTLMaterialManager> &materials);
    void addMaterial(const AMTLMaterialManager &material, const std::string &name = {});

    std::vector<AMTLMaterialManager>& getMaterials();
    AMTLMaterialManager& getMaterial(const std::string &name);

    bool isMaterialExists(const std::string &name) const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

    void importFromFile(const std::string &path) override;

private:
    std::vector<AMTLMaterialManager> m_materials;
    std::string m_workingDirectory;
};
}

#endif //ALGINE_AMTLMANAGER_H
