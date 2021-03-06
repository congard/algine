#ifndef ALGINE_SHADERPROGRAMCREATOR_H
#define ALGINE_SHADERPROGRAMCREATOR_H

#include <algine/core/shader/ShaderCreator.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/Creator.h>

namespace algine {
class ShaderProgramCreator: public ShaderDefinitionGenerator, public Creator {
public:
    void setShaders(const std::vector<ShaderCreator> &shaders);
    void addShader(const ShaderCreator &creator);
    const std::vector<ShaderCreator>& getShaders() const;

    void setShaderNames(const std::vector<std::string> &names);
    void addShaderName(const std::string &name);
    const std::vector<std::string>& getShaderNames() const;

    void setShaderPaths(const std::vector<std::string> &paths);
    void addShaderPath(const std::string &path);
    const std::vector<std::string>& getShaderPaths() const;

    ShaderProgramPtr get();
    ShaderProgramPtr create();

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

    void importFromFile(const std::string &path) override;

private:
    std::vector<ShaderCreator> m_shaders;
    std::vector<std::string> m_shaderNames, m_shaderPaths;
};
}

#endif //ALGINE_SHADERPROGRAMCREATOR_H
