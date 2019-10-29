#ifndef ALGINE_SHADER_H
#define ALGINE_SHADER_H

#include <algine/types.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <tulz/StringUtils>

namespace algine {
void getShaderInfoLog(int shader, int type);
void getProgramInfoLog(int program, int type);

struct ShadersData {
    std::string
        vertex,
        fragment,
        geometry = "";
};

#define ShadersSources ShadersData
#define ShadersPaths ShadersData

namespace ShaderType {
    enum {
        Vertex,
        Fragment,
        Geometry
    };
}

class ShaderManager {
public:
    enum {
        RemoveFirst,
        RemoveLast,
        RemoveAll
    };

    void fromFile(const std::string &vertex, const std::string &fragment, const std::string &geometry = std::string());
    void fromFile(const ShadersData &paths);
    void fromSource(const std::string &vertex, const std::string &fragment, const std::string &geometry = std::string());
    void fromSource(const ShadersData &sources);

    void setBaseIncludePath(const std::string &path, int shaderType = -1);
    void addIncludePath(const std::string &includePath);

    void define(const std::string &macro, const std::string &value = std::string(), int shaderType = -1); // -1: define in all shaders
    void removeDefinition(uint shaderType, const std::string &macro, uint type = RemoveLast);
    void removeDefinition(const std::string &macro, uint type = RemoveLast); // removes in all shaders

    void resetGenerated();
    void resetDefinitions();
    void generate();

    ShadersData getTemplate();
    ShadersData getGenerated();
    ShadersData makeGenerated();

protected:
    std::vector<std::pair<std::string, std::string>> definitions[3]; // 3 because vertex, fragment and geometry
    std::vector<std::string> includePaths;
    std::string baseIncludePath[3]; // vertex, fragment, geometry
    std::string vertexTemp, fragmentTemp, geometryTemp; // shaders; temp - template
    std::string vertexGen, fragmentGen, geometryGen; // gen - generated

    std::string processDirectives(const std::string &src, const std::string &baseIncludePath);
    static std::vector<tulz::StringUtils::Matches> findPragmas(const std::string &src, const std::string &regex);
};

class Shader {
public:
    uint id = 0;

    explicit Shader(uint type); // calls create(type)
    Shader();
    ~Shader();

    void create(uint type);
    void fromSource(const std::string &source);
    void fromFile(const std::string &path);
};

class ShaderProgram {
public:
    uint id;
    std::unordered_map<std::string, int> locations;

    ShaderProgram();
    ~ShaderProgram();

    void fromSource(const std::string &vertex, const std::string &fragment, const std::string &geometry = std::string()); // unnecessary can be left empty
    void fromSource(const ShadersSources &shaders); // calls fromSource(v, f, g)
    void fromFile(const std::string &vertex, const std::string &fragment, const std::string &geometry = std::string()); // unnecessary can be left empty
    void fromFile(const ShadersPaths &paths); // calls fromFile(v, f, g)
    void attachShader(const Shader &shader);
    void link();

    void loadUniformLocation(const std::string &name);
    void loadUniformLocations(const std::vector<std::string> &names);
    void loadAttribLocation(const std::string &name);
    void loadAttribLocations(const std::vector<std::string> &names);
    void loadActiveLocations();
    int getLocation(const std::string &name);

    void use();
    static void reset();
    static void set(int location, bool p);
    static void set(int location, int p);
    static void set(int location, uint p);
    static void set(int location, float p);
    static void set(int location, const glm::vec3 &p);
    static void set(int location, const glm::vec4 &p);
    static void set(int location, const glm::mat3 &p);
    static void set(int location, const glm::mat4 &p);

    template<typename...Args>
    static void create(Args&...args) {
        ShaderProgram** arr[] = {&args...};
        for (usize i = 0; i < sizeof...(args); i++)
            *arr[i] = new ShaderProgram();
    }

    template<typename T>
    void set(const std::string &location, const T &p) {
        set(getLocation(location), p);
    }
};
}

#endif // ALGINE_SHADER_H
