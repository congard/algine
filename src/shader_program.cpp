#include <algine/shader_program.h>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <algine/texture.h>
#include <algine/io.h>
#include <algine/core_utils.h>
#include <algine/constants.h>

namespace algine {
void getShaderInfoLog(int shader, int type) {
    GLint infoLogSize, success;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);
    
    if (infoLogSize == 0) return;

    GLchar infoLog[infoLogSize];
    glGetShaderiv(shader, type, &success);

    if (!success) {
        glGetShaderInfoLog(shader, infoLogSize, nullptr, infoLog);
        std::cout << "Shader info log (for shader id " << shader << "): " << infoLog << "\n";
    }
}

void getProgramInfoLog(int program, int type) {
    GLint infoLogSize, success;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
    
    if (infoLogSize == 0) return;

    GLchar infoLog[infoLogSize];
    glGetProgramiv(program, type, &success);

    if (!success) {
        glGetProgramInfoLog(program, infoLogSize, nullptr, infoLog);
        std::cout << "Program info log (for program id " << program << "): " << infoLog << "\n";
    }
}

void ShaderManager::fromFile(const std::string &vertex, const std::string &fragment, const std::string &geometry) {
    if (geometry.empty())
        fromSource(io::read(vertex), io::read(fragment));
    else
        fromSource(io::read(vertex), io::read(fragment), io::read(geometry));
}

void ShaderManager::fromFile(const algine::ShadersData &paths) {
    fromFile(paths.vertex, paths.fragment, paths.geometry);
}

void ShaderManager::fromSource(const std::string &vertex, const std::string &fragment, const std::string &geometry) {
    vertexTemp = vertex;
    fragmentTemp = fragment;
    geometryTemp = geometry;
    resetGenerated();
}

void ShaderManager::fromSource(const algine::ShadersData &sources) {
    fromSource(sources.vertex, sources.fragment, sources.geometry);
}

void ShaderManager::insert(const uint shaderType, const std::string &key, const std::string &code) {
    switch (shaderType) {
        case ShaderType::Vertex:
            vertexGen = replace(vertexGen, key, code);
            break;
        case ShaderType::Fragment:
            fragmentGen = replace(fragmentGen, key, code);
            break;
        case ShaderType::Geometry:
            geometryGen = replace(geometryGen, key, code);
            break;
        default:
            std::cerr << "Unknown shader type " << shaderType << "\n";
            return;
    }
}

void ShaderManager::define(const uint shaderType, const std::string &macro, const std::string &value) {
    if (shaderType > ShaderType::Geometry) {
        std::cerr << "Unknown shader type " << shaderType << "\n";
        return;
    }

    definitions[shaderType].emplace_back(macro, value);
}

void ShaderManager::define(const std::string &macro, const std::string &value) {
    define(ShaderType::Vertex, macro, value);
    define(ShaderType::Fragment, macro, value);
    define(ShaderType::Geometry, macro, value);
}

void ShaderManager::removeDefinition(const uint shaderType, const std::string &macro, const uint type) {
    switch (type) {
        case RemoveFirst:
        case RemoveAll:
            for (uint i = 0; i < static_cast<uint>(definitions[shaderType].size()); i++)
                if (definitions[shaderType][i].first == macro) {
                    definitions[shaderType].erase(definitions[shaderType].begin() + i);

                    if (shaderType == RemoveFirst)
                        return;
                }
            break;
        case RemoveLast:
            for (uint i = static_cast<uint>(definitions[shaderType].size()) - 1; i >= 0; i--)
                if (definitions[shaderType][i].first == macro) {
                    definitions[shaderType].erase(definitions[shaderType].begin() + i);
                    return;
                }
            break;
        default:
            std::cerr << "Unknown shader type " << shaderType << "\n";
            break;
    }
}

void ShaderManager::removeDefinition(const std::string &macro, const uint type) {
    removeDefinition(ShaderType::Vertex, macro, type);
    removeDefinition(ShaderType::Fragment, macro, type);
    removeDefinition(ShaderType::Geometry, macro, type);
}

void ShaderManager::resetGenerated() {
    vertexGen = vertexTemp;
    fragmentGen = fragmentTemp;
    geometryGen = geometryTemp;
}

void ShaderManager::resetDefinitions() {
    definitions[0].clear();
    definitions[1].clear();
    definitions[2].clear();
}

void ShaderManager::generate() {
    using namespace AlgineConstants;

    // generate definitions code
    std::string definitionsCode[3];
    for (uint i = 0; i < 3; i++) {
        for (uint j = 0; j < static_cast<uint>(definitions[i].size()); j++) {
            definitionsCode[i] += "#define " + definitions[i][j].first + " " + definitions[i][j].second + "\n";
        }
    }

    // insert definitions code
    insert(ShaderType::Vertex, ShaderGeneratorKeys::Definitions, definitionsCode[ShaderType::Vertex]);
    insert(ShaderType::Fragment, ShaderGeneratorKeys::Definitions, definitionsCode[ShaderType::Fragment]);
    insert(ShaderType::Geometry, ShaderGeneratorKeys::Definitions, definitionsCode[ShaderType::Geometry]);
}

ShadersData ShaderManager::getTemplate() {
    return ShadersData {
        vertexTemp,
        fragmentTemp,
        geometryTemp
    };
}

ShadersData ShaderManager::getGenerated() {
    return ShadersData {
        vertexGen,
        fragmentGen,
        geometryGen
    };
}

Shader::Shader(const uint type) {
    create(type);
}

Shader::Shader() = default;

Shader::~Shader() {
    glDeleteShader(id);
}

void Shader::create(const uint type) {
    switch (type) {
        case ShaderType::Vertex:
            id = glCreateShader(GL_VERTEX_SHADER);
            break;
        case ShaderType::Fragment:
            id = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        case ShaderType::Geometry:
            id = glCreateShader(GL_GEOMETRY_SHADER);
            break;
        default:
            std::cerr << "Unknown shader type " << type << "\n";
    }
}

void Shader::fromSource(const std::string &source) {
    const char *c_str = source.c_str();
    glShaderSource(id, 1, &c_str, nullptr);
    glCompileShader(id);
    getShaderInfoLog(id, GL_COMPILE_STATUS);
}

void Shader::fromFile(const std::string &path) {
    fromSource(io::read(path));
}

ShaderProgram::ShaderProgram() {
    id = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id);
}

void ShaderProgram::fromSource(const std::string &vertex, const std::string &fragment, const std::string &geometry) {
    if (!vertex.empty()) {
        Shader s_vertex(ShaderType::Vertex);
        s_vertex.fromSource(vertex);
        attachShader(s_vertex);
    }

    if (!fragment.empty()) {
        Shader s_fragment(ShaderType::Fragment);
        s_fragment.fromSource(fragment);
        attachShader(s_fragment);
    }

    if (!geometry.empty()) {
        Shader s_geometry(ShaderType::Geometry);
        s_geometry.fromSource(geometry);
        attachShader(s_geometry);
    }

    link();
}

void ShaderProgram::fromSource(const ShadersSources &shaders) {
    fromSource(shaders.vertex, shaders.fragment, shaders.geometry);
}

void ShaderProgram::fromFile(const std::string &vertex, const std::string &fragment, const std::string &geometry) {
    std::string vertexSource = vertex.empty() ? "" : io::read(vertex);
    std::string fragmentSource = fragment.empty() ? "" : io::read(fragment);
    std::string geometrySource = geometry.empty() ? "" : io::read(geometry);
    fromSource(vertexSource, fragmentSource, geometrySource);
}

void ShaderProgram::fromFile(const ShadersPaths &paths) {
    fromFile(paths.vertex, paths.fragment, paths.geometry);
}

void ShaderProgram::attachShader(const Shader &shader) {
    glAttachShader(id, shader.id);
}

void ShaderProgram::link() {
    glLinkProgram(id);
    getProgramInfoLog(id, GL_LINK_STATUS);
}

void ShaderProgram::loadUniformLocation(const std::string &name) {
    locations[name] = glGetUniformLocation(id, name.c_str());
}

void ShaderProgram::loadUniformLocations(const std::vector<std::string> &names) {
    for (const std::string &name : names)
        loadUniformLocation(name);
}

void ShaderProgram::loadAttribLocation(const std::string &name) {
    locations[name] = glGetAttribLocation(id, name.c_str());
}

void ShaderProgram::loadAttribLocations(const std::vector<std::string> &names) {
    for (const std::string &name : names)
        loadAttribLocation(name);
}

int ShaderProgram::getLocation(const std::string &name) {
    return locations[name];
}

void ShaderProgram::use() {
    glUseProgram(id);
}

void ShaderProgram::reset() {
    glUseProgram(0);
}

void ShaderProgram::set(const int location, const bool p) {
    glUniform1i(location, p);
}

void ShaderProgram::set(const int location, const int p) {
    glUniform1i(location, p);
}

void ShaderProgram::set(const int location, const uint p) {
    glUniform1ui(location, p);
}

void ShaderProgram::set(const int location, const float p) {
    glUniform1f(location, p);
}

void ShaderProgram::set(const int location, const glm::vec3 &p) {
    glUniform3fv(location, 1, glm::value_ptr(p));
}

void ShaderProgram::set(const int location, const glm::vec4 &p) {
    glUniform4fv(location, 1, glm::value_ptr(p));
}

void ShaderProgram::set(const int location, const glm::mat3 &p) {
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(p));
}

void ShaderProgram::set(const int location, const glm::mat4 &p) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(p));
}
}
