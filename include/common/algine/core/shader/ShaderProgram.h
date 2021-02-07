#ifndef ALGINE_SHADERPROGRAM_H
#define ALGINE_SHADERPROGRAM_H

#include <algine/core/shader/ShaderProgramPtr.h>
#include <algine/core/shader/ShadersInfo.h>
#include <algine/core/shader/Shader.h>

#include <algine/types.h>
#include <algine/templates.h>

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <unordered_map>
#include <string>
#include <vector>

namespace algine {
class ShaderProgram: public Object {
public:
    ShaderProgram();
    ~ShaderProgram();

    void fromSource(const std::string &vertex, const std::string &fragment, const std::string &geometry = std::string()); // unnecessary can be left empty
    void fromSource(const ShadersInfo &shaders); // calls fromSource(v, f, g)
    void fromFile(const std::string &vertex, const std::string &fragment, const std::string &geometry = std::string()); // unnecessary can be left empty
    void fromFile(const ShadersInfo &paths); // calls fromFile(v, f, g)
    void attachShader(const Shader &shader);
    void link();

    void loadUniformLocation(const std::string &name);
    void loadUniformLocations(const std::vector<std::string> &names);
    void loadAttribLocation(const std::string &name);
    void loadAttribLocations(const std::vector<std::string> &names);
    void loadActiveLocations();
    int getLocation(const std::string &name);

    void bind();
    void unbind();
    static void setBool(int location, bool p);
    static void setInt(int location, int p);
    static void setUint(int location, uint p);
    static void setFloat(int location, float p);
    static void setVec3(int location, const glm::vec3 &p);
    static void setVec4(int location, const glm::vec4 &p);
    static void setMat3(int location, const glm::mat3 &p);
    static void setMat4(int location, const glm::mat4 &p);

    void setBool(const std::string &location, bool p);
    void setInt(const std::string &location, int p);
    void setUint(const std::string &location, uint p);
    void setFloat(const std::string &location, float p);
    void setVec3(const std::string &location, const glm::vec3 &p);
    void setVec4(const std::string &location, const glm::vec4 &p);
    void setMat3(const std::string &location, const glm::mat3 &p);
    void setMat4(const std::string &location, const glm::mat4 &p);

    uint getId() const;

    implementVariadicCreate(ShaderProgram)
    implementVariadicDestroy(ShaderProgram)

public: // TODO: make private
    uint id;
    std::unordered_map<std::string, int> locations;

public:
    static ShaderProgramPtr getByName(const std::string &name);
    static ShaderProgram* byName(const std::string &name);

public:
    static std::vector<ShaderProgramPtr> publicObjects;
};
}

#endif //ALGINE_SHADERPROGRAM_H
