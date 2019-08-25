#ifndef ALGINE_SHADERPROGRAM_H
#define ALGINE_SHADERPROGRAM_H

#include <algine/types.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

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

class Shader {
public:
    enum {
        Vertex,
        Fragment,
        Geometry
    };

    uint id;

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

    template<typename T>
    void set(const std::string &location, const T &p) {
        set(getLocation(location), p);
    }
};
}

#endif
