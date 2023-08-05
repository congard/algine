#include <algine/core/shader/ShaderProgram.h>
#include <algine/core/shader/ShaderTools.h>
#include <algine/core/Engine.h>
#include <algine/gl.h>

#include <tulz/File.h>
#include <tulz/macros.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>

using namespace std;
using namespace tulz;

namespace algine {
AL_CONTEXT_OBJECT_DEFAULT_INITIALIZER(ShaderProgram) {
    .obj = []() {
        auto program = new ShaderProgram(std::false_type {});
        program->m_id = 0;
        return program;
    }()
};

ShaderProgram::ShaderProgram(Object *parent)
    : ContextObject(parent)
{
    m_id = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(m_id);
}

void ShaderProgram::fromSource(const string &vertex, const string &fragment, const string &geometry) {
    // TODO: maybe these shaders should have `this` as parent?
    //  then allocate them in a heap

    if (!vertex.empty()) {
        Shader s_vertex(Shader::Type::Vertex, nullptr);
        s_vertex.fromSource(vertex);
        attachShader(s_vertex);
    }

    if (!fragment.empty()) {
        Shader s_fragment(Shader::Type::Fragment, nullptr);
        s_fragment.fromSource(fragment);
        attachShader(s_fragment);
    }

    if (!geometry.empty()) {
        Shader s_geometry(Shader::Type::Geometry, nullptr);
        s_geometry.fromSource(geometry);
        attachShader(s_geometry);
    }

    link();
}

void ShaderProgram::fromSource(const ShadersInfo &shaders) {
    fromSource(shaders.vertex, shaders.fragment, shaders.geometry);
}

void ShaderProgram::fromFile(const string &vertex, const string &fragment, const string &geometry) {
    string vertexSource = vertex.empty() ? "" : File(vertex, File::Mode::ReadText).readStr();
    string fragmentSource = fragment.empty() ? "" : File(fragment, File::Mode::ReadText).readStr();
    string geometrySource = geometry.empty() ? "" : File(geometry, File::Mode::ReadText).readStr();
    fromSource(vertexSource, fragmentSource, geometrySource);
}

void ShaderProgram::fromFile(const ShadersInfo &paths) {
    fromFile(paths.vertex, paths.fragment, paths.geometry);
}

void ShaderProgram::attachShader(const Shader &shader) {
    glAttachShader(m_id, shader.getId());
}

void ShaderProgram::link() {
    glLinkProgram(m_id);

    string infoLog = ShaderTools::getProgramInfoLogById(m_id, GL_LINK_STATUS);

    if (!infoLog.empty()) {
        cerr << "Info log of ShaderProgram with id " << m_id << ": " << infoLog;
    }
}

void ShaderProgram::loadUniformLocation(const string &name) {
    if (m_locations.find(name) == m_locations.end()) {
        m_locations[name] = glGetUniformLocation(m_id, name.c_str());
    }
}

void ShaderProgram::loadUniformLocations(const vector<string> &names) {
    for (const string &name : names) {
        loadUniformLocation(name);
    }
}

void ShaderProgram::loadAttribLocation(const string &name) {
    if (m_locations.find(name) == m_locations.end()) {
        m_locations[name] = glGetAttribLocation(m_id, name.c_str());
    }
}

void ShaderProgram::loadAttribLocations(const vector<string> &names) {
    for (const string &name : names) {
        loadAttribLocation(name);
    }
}

void ShaderProgram::loadActiveLocations() {
    int numActiveAttribs = 0;
    int numActiveUniforms = 0;

    glGetProgramInterfaceiv(m_id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
    glGetProgramInterfaceiv(m_id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);

    vector<char> nameData;
    uint properties[] = {GL_NAME_LENGTH};
    int values[getArraySize(properties)];

    auto getName = [&](uint type, uint index)
    {
        glGetProgramResourceiv(m_id, type, index, getArraySize(properties),
                               &properties[0], getArraySize(values), nullptr, &values[0]);

        nameData.resize(values[0]); // the length of the name

        glGetProgramResourceName(m_id, type, index, nameData.size(), nullptr, &nameData[0]);

        return string(&nameData[0], nameData.size() - 1);
    };

    for (uint i = 0; i < numActiveAttribs; ++i) {
        loadAttribLocation(getName(GL_PROGRAM_INPUT, i));
    }

    for (uint i = 0; i < numActiveUniforms; ++i) {
        loadUniformLocation(getName(GL_UNIFORM, i));
    }
}

int ShaderProgram::getLocation(const string &name) {
#ifdef ALGINE_SECURE_OPERATIONS
    auto it = m_locations.find(name);

    if (it == m_locations.end()) {
        ContextObjectImpl<ShaderProgram>::throwError(
            "Variable " + name + " does not exist in program with id " + to_string(m_id) + "\n"
            "Maybe you have forgotten to call loadActiveLocations() first?"
        );
    } else {
        return it->second;
    }

    return -1;
#else
    return m_locations[name];
#endif
}

void ShaderProgram::bind() {
    commitBinding();
    glUseProgram(m_id);
}

void ShaderProgram::unbind() {
    checkBinding();
    commitUnbinding();
    glUseProgram(0);
}

void ShaderProgram::setBool(int location, bool p) {
    glUniform1i(location, p);
}

void ShaderProgram::setInt(int location, int p) {
    glUniform1i(location, p);
}

void ShaderProgram::setUint(int location, uint p) {
    glUniform1ui(location, p);
}

void ShaderProgram::setFloat(int location, float p) {
    glUniform1f(location, p);
}

void ShaderProgram::setVec2(int location, const glm::vec2 &p) {
    glUniform2fv(location, 1, glm::value_ptr(p));
}

void ShaderProgram::setVec2(int location, float p0, float p1) {
    glUniform2f(location, p0, p1);
}

void ShaderProgram::setVec3(int location, const glm::vec3 &p) {
    glUniform3fv(location, 1, glm::value_ptr(p));
}

void ShaderProgram::setVec3(int location, float p0, float p1, float p2) {
    glUniform3f(location, p0, p1, p2);
}

void ShaderProgram::setVec4(int location, const glm::vec4 &p) {
    glUniform4fv(location, 1, glm::value_ptr(p));
}

void ShaderProgram::setVec4(int location, float p0, float p1, float p2, float p3) {
    glUniform4f(location, p0, p1, p2, p3);
}

void ShaderProgram::setMat3(int location, const glm::mat3 &p) {
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(p));
}

void ShaderProgram::setMat4(int location, const glm::mat4 &p) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(p));
}

void ShaderProgram::setBool(const string &location, bool p) {
    checkBinding();
    setBool(getLocation(location), p);
}

void ShaderProgram::setInt(const string &location, int p) {
    checkBinding();
    setInt(getLocation(location), p);
}

void ShaderProgram::setUint(const string &location, uint p) {
    checkBinding();
    setUint(getLocation(location), p);
}

void ShaderProgram::setFloat(const string &location, float p) {
    checkBinding();
    setFloat(getLocation(location), p);
}

void ShaderProgram::setVec2(const string &location, const glm::vec2 &p) {
    checkBinding();
    setVec2(getLocation(location), p);
}

void ShaderProgram::setVec2(const std::string &location, float p0, float p1) {
    checkBinding();
    setVec2(getLocation(location), p0, p1);
}

void ShaderProgram::setVec3(const string &location, const glm::vec3 &p) {
    checkBinding();
    setVec3(getLocation(location), p);
}

void ShaderProgram::setVec3(const std::string &location, float p0, float p1, float p2) {
    checkBinding();
    setVec3(getLocation(location), p0, p1, p2);
}

void ShaderProgram::setVec4(const string &location, const glm::vec4 &p) {
    checkBinding();
    setVec4(getLocation(location), p);
}

void ShaderProgram::setVec4(const std::string &location, float p0, float p1, float p2, float p3) {
    checkBinding();
    setVec4(getLocation(location), p0, p1, p2, p3);
}

void ShaderProgram::setMat3(const string &location, const glm::mat3 &p) {
    checkBinding();
    setMat3(getLocation(location), p);
}

void ShaderProgram::setMat4(const string &location, const glm::mat4 &p) {
    checkBinding();
    setMat4(getLocation(location), p);
}
}
