#ifndef ALGINE_SHADERPROGRAMMANAGER_H
#define ALGINE_SHADERPROGRAMMANAGER_H

#include <algine/core/shader/ShaderProgramPtr.h>
#include <algine/core/shader/ShaderManager.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/transfer/DumpModeHelper.h>
#include <algine/core/ManagerBase.h>

namespace algine {
class ShaderProgramManager: public ShaderDefinitionManager, public ManagerBase {
public:
    using PrivateShader = DumpModeHelper<ShaderManager>;

public:
    void setPrivateShaders(const std::vector<PrivateShader> &shaders);
    void addPrivateShader(const ShaderManager &manager, uint dumpMode = PrivateShader::Dump);
    const std::vector<PrivateShader>& getPrivateShaders() const;

    void setPublicShaders(const std::vector<std::string> &shaders);
    void addPublicShader(const std::string &name);
    const std::vector<std::string>& getPublicShaders() const;

    ShaderProgramPtr createProgram();

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

    void importFromFile(const std::string &path) override;

private:
    std::vector<PrivateShader> m_privateShaders;
    std::vector<std::string> m_publicShaders;
};
}

#endif //ALGINE_SHADERPROGRAMMANAGER_H
