#include <algine/std/AMTLManager.h>

#include <algine/core/JsonHelper.h>

using namespace std;
using namespace nlohmann;

namespace algine {
void AMTLManager::setMaterials(const vector<AMTLMaterialManager> &materials) {
    m_materials = materials;
}

void AMTLManager::addMaterial(const AMTLMaterialManager &material, const string &name) {
    if (name.empty()) {
        m_materials.emplace_back(material);
    } else {
        auto customNamedMaterial = material;
        customNamedMaterial.setName(name);
        m_materials.emplace_back(customNamedMaterial);
    }
}

vector<AMTLMaterialManager>& AMTLManager::getMaterials() {
    return m_materials;
}

inline int getMaterialIndex(const string &name, const vector<AMTLMaterialManager> &materials) {
    for (int i = 0; i < materials.size(); i++) {
        if (materials[i].getName() == name) {
            return i;
        }
    }

    return -1;
}

AMTLMaterialManager& AMTLManager::getMaterial(const string &name) {
    int index = getMaterialIndex(name, m_materials);

    if (index != -1)
        return m_materials[index];

    throw runtime_error("AMTLMaterial '" + name + "' does not found");
}

bool AMTLManager::isMaterialExists(const string &name) const {
    return getMaterialIndex(name, m_materials) != -1;
}

void AMTLManager::import(const JsonHelper &jsonHelper) {
    const json &config = jsonHelper.json;

    for (const auto & material : config) {
        AMTLMaterialManager materialManager;
        materialManager.m_workingDirectory = m_workingDirectory;
        materialManager.import(material);
        m_materials.emplace_back(materialManager);
    }
}

JsonHelper AMTLManager::dump() {
    json config;

    for (auto & material : m_materials)
        config.push_back(material.dump().json);

    return config;
}
}
