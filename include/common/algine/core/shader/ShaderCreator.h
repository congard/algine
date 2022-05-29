#ifndef ALGINE_SHADERCREATOR_H
#define ALGINE_SHADERCREATOR_H

#include <algine/core/shader/ShaderDefinitionGenerator.h>
#include <algine/core/shader/ShaderPtr.h>
#include <algine/core/shader/Shader.h>
#include <algine/core/Creator.h>
#include <algine/types.h>

#include <memory>
#include <vector>

namespace tulz {
class Path;
}

namespace algine {
class ShaderCreator: public ShaderDefinitionGenerator, public Creator {
public:
    ShaderCreator();
    explicit ShaderCreator(Shader::Type type);

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

    ShaderPtr get();
    ShaderPtr create();

    void dumperUseSources(bool use);

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

    void importFromFile(const std::string &path) override;

    static void registerLuaUsertype(Lua *lua);

protected:
    void exec(const std::string &s, bool path, Lua *lua) override;

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
    bool m_dumperUseSources;

private:
    static std::vector<std::string> m_globalIncludePaths;
};
}

#endif //ALGINE_SHADERCREATOR_H
