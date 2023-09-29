#ifndef ALGINE_SHADERBUILDER_H
#define ALGINE_SHADERBUILDER_H

#include <algine/core/shader/ShaderDefinitionGenerator.h>
#include <algine/core/shader/Shader.h>
#include <algine/core/Builder.h>
#include <algine/types.h>

#include <memory>
#include <vector>

namespace tulz {
class Path;
}

namespace algine {
class AL_EXPORT ShaderBuilder: public ShaderDefinitionGenerator, public Builder {
    AL_BUILDER(Shader)

public:
    ShaderBuilder();
    ShaderBuilder(Shader::Type type, const std::string &path);
    explicit ShaderBuilder(Shader::Type type);
    explicit ShaderBuilder(const std::string &path);

    void setType(Shader::Type type);
    Shader::Type getType() const;

    void setPath(const std::string &path);
    void setIncludePaths(const std::vector<std::string> &includePaths);
    void addIncludePaths(const std::vector<std::string> &includePaths);
    void addIncludePath(const std::string &includePath);
    void removeIncludePath(const std::string &includePath);

    const std::string& getPath() const;
    const std::vector<std::string>& getIncludePaths() const;

    void setSource(const std::string &source);
    const std::string& getSource() const;

    void resetGenerated();
    void generate();

    const std::string& getGenerated() const;
    const std::string& makeGenerated();

protected:
    Object* createImpl() override;
    void exec(const std::string &s, bool path, Lua *lua, sol::environment *tenv) override;

public:
    static void setGlobalIncludePaths(const std::vector<std::string> &includePaths);
    static void addGlobalIncludePaths(const std::vector<std::string> &includePaths);
    static void addGlobalIncludePath(const std::string &includePath);
    static void removeGlobalIncludePath(const std::string &includePath);

    static std::vector<std::string>& getGlobalIncludePaths();

private:
    std::string processDirectives(const std::string &src, const tulz::Path &baseIncludePath);

private:
    std::vector<std::string> m_includePaths;
    std::string m_path;
    std::string m_source;
    std::string m_gen;
    Shader::Type m_type;

private:
    static std::vector<std::string> m_globalIncludePaths;
};
}

#endif //ALGINE_SHADERBUILDER_H
