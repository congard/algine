#ifndef ALGINE_SHADERPROGRAMCREATOR_H
#define ALGINE_SHADERPROGRAMCREATOR_H

#include <algine/core/shader/ShaderCreator.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/Creator.h>

namespace algine {
class ShaderProgramCreator: public ShaderDefinitionGenerator, public Creator {
public:
    void setCreators(const std::vector<ShaderCreator> &shaders);
    void addCreator(const ShaderCreator &creator);
    const std::vector<ShaderCreator>& getCreators() const;

    void setShaderNames(const std::vector<std::string> &names);
    void addShaderName(const std::string &name);
    const std::vector<std::string>& getShaderNames() const;

    ShaderProgramPtr get();
    ShaderProgramPtr create();

    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv);

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *env) override;

private:
    std::vector<ShaderCreator> m_shaders;
    std::vector<std::string> m_shaderNames;
};
}

#endif //ALGINE_SHADERPROGRAMCREATOR_H
