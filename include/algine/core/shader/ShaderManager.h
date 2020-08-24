#ifndef ALGINE_SHADERMANAGER_H
#define ALGINE_SHADERMANAGER_H

#include <algine/core/shader/ShaderDefinitionManager.h>
#include <algine/core/shader/Shader.h>
#include <algine/types.h>

#include <memory>
#include <vector>

namespace algine {
class ShaderManager: public ShaderDefinitionManager {
public:
    enum {
        Private,
        Public
    };

public:
    void fromFile(const std::string &path);
    void fromSource(const std::string &source);

    void setBaseIncludePath(const std::string &path);
    void addIncludePath(const std::string &includePath);

    void resetGenerated();
    void generate();

    void setName(const std::string &name);
    void setType(uint type);

    std::string getName() const;
    uint getType() const;

    std::string getTemplate();
    std::string getGenerated();
    std::string makeGenerated();

    std::shared_ptr<Shader> createShader(uint access = Private);

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    std::string processDirectives(const std::string &src, const std::string &baseIncludePath);

private:
    std::vector<std::string> m_includePaths;
    std::string m_path;
    std::string m_baseIncludePath;
    std::string m_source;
    std::string m_gen;
    std::string m_name;
    uint m_type;
    bool m_configUseSources = false;
};
}

#endif //ALGINE_SHADERMANAGER_H
