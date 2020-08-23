#ifndef ALGINE_SHADERMANAGER_H
#define ALGINE_SHADERMANAGER_H

#include <algine/core/shader/ShadersInfo.h>
#include <algine/core/transfer/Transferable.h>
#include <algine/types.h>

#include <vector>

// TODO: generate only one shader, not vertex/fragment/geometry

namespace algine {
class JsonHelper;

class ShaderManager: public Transferable {
public:
    enum {
        RemoveFirst,
        RemoveLast,
        RemoveAll
    };

public:
    void fromFile(const std::string &vertex, const std::string &fragment, const std::string &geometry = std::string());
    void fromFile(const ShadersInfo &paths);
    void fromSource(const std::string &vertex, const std::string &fragment, const std::string &geometry = std::string());
    void fromSource(const ShadersInfo &sources);

    void setBaseIncludePath(const std::string &path, int shaderType = -1);
    void addIncludePath(const std::string &includePath);

    void define(const std::string &macro, const std::string &value = std::string(), int shaderType = -1); // -1: define in all shaders
    void define(const std::string &macro, size value, int shaderType = -1); // -1: define in all shaders
    void removeDefinition(uint shaderType, const std::string &macro, uint type = RemoveLast);
    void removeDefinition(const std::string &macro, uint type = RemoveLast); // removes in all shaders

    void resetGenerated();
    void resetDefinitions();
    void generate();

    ShadersInfo getTemplate();
    ShadersInfo getGenerated();
    ShadersInfo makeGenerated();

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    typedef std::pair<std::string, std::string> Definition;

private:
    std::string processDirectives(const std::string &src, const std::string &baseIncludePath);

private:
    std::vector<Definition> m_definitions[3]; // 3 because vertex, fragment and geometry
    std::vector<std::string> m_includePaths;
    std::string m_shaderPaths[3]; // vertex, fragment, geometry
    std::string m_baseIncludePath[3]; // vertex, fragment, geometry
    std::string m_vertexTemp, m_fragmentTemp, m_geometryTemp; // shaders; temp - template
    std::string m_vertexGen, m_fragmentGen, m_geometryGen; // gen - generated

private:
    bool m_configUseSources = false;
};
}

#endif //ALGINE_SHADERMANAGER_H
