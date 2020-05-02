#ifndef ALGINE_SHADERMANAGER_H
#define ALGINE_SHADERMANAGER_H

#include <algine/core/ShadersInfo.h>

#include <vector>

namespace algine {
class ShaderManager {
public:
    enum {
        RemoveFirst,
        RemoveLast,
        RemoveAll
    };

    void fromFile(const std::string &vertex, const std::string &fragment, const std::string &geometry = std::string());
    void fromFile(const ShadersInfo &paths);
    void fromSource(const std::string &vertex, const std::string &fragment, const std::string &geometry = std::string());
    void fromSource(const ShadersInfo &sources);

    void setBaseIncludePath(const std::string &path, int shaderType = -1);
    void addIncludePath(const std::string &includePath);

    void define(const std::string &macro, const std::string &value = std::string(), int shaderType = -1); // -1: define in all shaders
    void removeDefinition(uint shaderType, const std::string &macro, uint type = RemoveLast);
    void removeDefinition(const std::string &macro, uint type = RemoveLast); // removes in all shaders

    void resetGenerated();
    void resetDefinitions();
    void generate();

    ShadersInfo getTemplate();
    ShadersInfo getGenerated();
    ShadersInfo makeGenerated();

protected:
    std::vector<std::pair<std::string, std::string>> definitions[3]; // 3 because vertex, fragment and geometry
    std::vector<std::string> includePaths;
    std::string baseIncludePath[3]; // vertex, fragment, geometry
    std::string vertexTemp, fragmentTemp, geometryTemp; // shaders; temp - template
    std::string vertexGen, fragmentGen, geometryGen; // gen - generated

    std::string processDirectives(const std::string &src, const std::string &baseIncludePath);
};
}

#endif //ALGINE_SHADERMANAGER_H
