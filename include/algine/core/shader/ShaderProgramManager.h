#ifndef ALGINE_SHADERPROGRAMMANAGER_H
#define ALGINE_SHADERPROGRAMMANAGER_H

#include <algine/core/shader/ShaderManager.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/ManagerBase.h>

namespace algine {
class ShaderProgramManager: public ShaderDefinitionManager, public ManagerBase {
public:
    void setShaders(const std::vector<ShaderManager> &shaders);
    void addShader(const ShaderManager &manager);
    const std::vector<ShaderManager>& getShaders() const;

    void setShaderNames(const std::vector<std::string> &names);
    void addShaderName(const std::string &name);
    const std::vector<std::string>& getShaderNames() const;

    void setShaderPaths(const std::vector<std::string> &paths);
    void addShaderPath(const std::string &path);
    const std::vector<std::string>& getShaderPaths() const;

    ShaderProgramPtr getProgram();
    ShaderProgramPtr createProgram();

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

    void importFromFile(const std::string &path) override;

private:
    std::vector<ShaderManager> m_shaders;
    std::vector<std::string> m_shaderNames, m_shaderPaths;
};
}

#endif //ALGINE_SHADERPROGRAMMANAGER_H
