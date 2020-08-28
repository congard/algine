#ifndef ALGINE_SHADERMANAGER_H
#define ALGINE_SHADERMANAGER_H

#include <algine/core/shader/ShaderDefinitionManager.h>
#include <algine/core/shader/ShaderPtr.h>
#include <algine/core/shader/Shader.h>
#include <algine/core/ManagerBase.h>
#include <algine/types.h>

#include <memory>
#include <vector>

namespace algine {
class ShaderManager: public ShaderDefinitionManager, public ManagerBase {
public:
    ShaderManager();
    explicit ShaderManager(uint type);

    void fromFile(const std::string &path);
    void fromSource(const std::string &source);

    void setIncludePaths(const std::vector<std::string> &includePaths);
    void addIncludePaths(const std::vector<std::string> &includePaths);
    void addIncludePath(const std::string &includePath);
    void removeIncludePath(const std::string &includePath);

    void resetGenerated();
    void generate();

    void setType(uint type);

    std::string getConfigPath() const;
    uint getType() const;

    const std::vector<std::string>& getIncludePaths() const;

    std::string getTemplate();
    std::string getGenerated();
    std::string makeGenerated();

    ShaderPtr createShader();

    void dumperUseSources(bool use);

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

    void importFromFile(const std::string &path) override;

public:
    static void setGlobalIncludePaths(const std::vector<std::string> &includePaths);
    static void addGlobalIncludePaths(const std::vector<std::string> &includePaths);
    static void addGlobalIncludePath(const std::string &includePath);
    static void removeGlobalIncludePath(const std::string &includePath);

    static std::vector<std::string>& getGlobalIncludePaths();

private:
    std::string processDirectives(const std::string &src, const std::string &baseIncludePath);

private:
    std::vector<std::string> m_includePaths;
    std::string m_path;
    std::string m_source;
    std::string m_gen;
    std::string m_confPath;
    uint m_type;
    bool m_dumperUseSources;

private:
    static std::vector<std::string> m_globalIncludePaths;
};
}

#endif //ALGINE_SHADERMANAGER_H
