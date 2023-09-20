#ifndef ALGINE_SHADERPROGRAMBUILDER_H
#define ALGINE_SHADERPROGRAMBUILDER_H

#include <algine/core/shader/ShaderBuilder.h>
#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/Builder.h>

namespace algine {
class AL_EXPORT ShaderProgramBuilder: public ShaderDefinitionGenerator, public Builder {
    AL_BUILDER(ShaderProgram)

public:
    void setBuilders(const std::vector<ShaderBuilder> &shaders);
    void addBuilder(const ShaderBuilder &builder);
    const std::vector<ShaderBuilder>& getBuilders() const;

    void setShaderNames(const std::vector<std::string> &names);
    void addShaderName(const std::string &name);
    const std::vector<std::string>& getShaderNames() const;

protected:
    Object* createImpl() override;
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *env) override;

private:
    std::vector<ShaderBuilder> m_shaders;
    std::vector<std::string> m_shaderNames;
};
}

#endif //ALGINE_SHADERPROGRAMBUILDER_H
