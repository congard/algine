#ifndef ALGINE_SHADERMANAGER_H
#define ALGINE_SHADERMANAGER_H

#include <algine/core/shader/ShaderDefinitionManager.h>
#include <algine/core/shader/Shader.h>
#include <algine/core/ManagerBase.h>
#include <algine/types.h>

#include <memory>
#include <vector>

namespace algine {
class ShaderManager: public ShaderDefinitionManager, public ManagerBase {
public:
    ShaderManager();

    void fromFile(const std::string &path);
    void fromSource(const std::string &source);

    void setBaseIncludePath(const std::string &path);
    void addIncludePath(const std::string &includePath);

    void resetGenerated();
    void generate();

    void setType(uint type);

    std::string getConfigPath() const;
    uint getType() const;

    std::string getTemplate();
    std::string getGenerated();
    std::string makeGenerated();

    std::shared_ptr<Shader> createShader();

    void dumperUseSources(bool use);

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

    void importFromFile(const std::string &path) override;

private:
    std::string processDirectives(const std::string &src, const std::string &baseIncludePath);

private:
    std::vector<std::string> m_includePaths;
    std::string m_path;
    std::string m_baseIncludePath;
    std::string m_source;
    std::string m_gen;
    std::string m_confPath;
    uint m_type;
    bool m_dumperUseSources;
};
}

#endif //ALGINE_SHADERMANAGER_H
