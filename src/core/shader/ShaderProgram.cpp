#include <algine/core/shader/ShaderProgram.h>

#include <algine/core/shader/ShaderTools.h>
#include <algine/core/Engine.h>

#include <tulz/File.h>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include <iostream>

#define SOP_BOUND_PTR Engine::m_boundShaderProgram
#define SOP_OBJECT_TYPE SOPConstants::ShaderProgramObject
#define SOP_OBJECT_ID id
#define SOP_OBJECT_NAME SOPConstants::ShaderProgramStr
#include "../SOP.h"
#include "../SOPConstants.h"

#include "../PublicObjectTools.h"

using namespace std;
using namespace tulz;

namespace algine {
vector<ShaderProgramPtr> ShaderProgram::publicObjects;

ShaderProgram::ShaderProgram() {
    id = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id);
}

void ShaderProgram::fromSource(const string &vertex, const string &fragment, const string &geometry) {
    if (!vertex.empty()) {
        Shader s_vertex(Shader::Vertex);
        s_vertex.fromSource(vertex);
        attachShader(s_vertex);
    }

    if (!fragment.empty()) {
        Shader s_fragment(Shader::Fragment);
        s_fragment.fromSource(fragment);
        attachShader(s_fragment);
    }

    if (!geometry.empty()) {
        Shader s_geometry(Shader::Geometry);
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
    glAttachShader(id, shader.getId());
}

void ShaderProgram::link() {
    glLinkProgram(id);

    string infoLog = ShaderTools::getProgramInfoLogById(id, GL_LINK_STATUS);

    if (!infoLog.empty()) {
        cerr << "Info log of ShaderProgram with id " << id << ": " << infoLog;
    }
}

void ShaderProgram::loadUniformLocation(const string &name) {
    if (locations.find(name) == locations.end()) {
        locations[name] = glGetUniformLocation(id, name.c_str());
    }
}

void ShaderProgram::loadUniformLocations(const vector<string> &names) {
    for (const string &name : names) {
        loadUniformLocation(name);
    }
}

void ShaderProgram::loadAttribLocation(const string &name) {
    if (locations.find(name) == locations.end()) {
        locations[name] = glGetAttribLocation(id, name.c_str());
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

    glGetProgramInterfaceiv(id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
    glGetProgramInterfaceiv(id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);

    vector<char> nameData;
    uint properties[] = {GL_NAME_LENGTH};
    int values[getArraySize(properties)];

    auto getName = [&](uint type, uint index)
    {
        glGetProgramResourceiv(id, type, index, getArraySize(properties),
                               &properties[0], getArraySize(values), nullptr, &values[0]);

        nameData.resize(values[0]); // the length of the name

        glGetProgramResourceName(id, type, index, nameData.size(), nullptr, &nameData[0]);

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
    return locations[name];
}

void ShaderProgram::bind() {
    commitBinding()
    glUseProgram(id);
}

void ShaderProgram::unbind() {
    checkBinding()
    commitUnbinding()
    glUseProgram(0);
}

void ShaderProgram::setBool(const int location, const bool p) {
    glUniform1i(location, p);
}

void ShaderProgram::setInt(const int location, const int p) {
    glUniform1i(location, p);
}

void ShaderProgram::setUint(const int location, const uint p) {
    glUniform1ui(location, p);
}

void ShaderProgram::setFloat(const int location, const float p) {
    glUniform1f(location, p);
}

void ShaderProgram::setVec3(const int location, const glm::vec3 &p) {
    glUniform3fv(location, 1, glm::value_ptr(p));
}

void ShaderProgram::setVec4(const int location, const glm::vec4 &p) {
    glUniform4fv(location, 1, glm::value_ptr(p));
}

void ShaderProgram::setMat3(const int location, const glm::mat3 &p) {
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(p));
}

void ShaderProgram::setMat4(const int location, const glm::mat4 &p) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(p));
}

void ShaderProgram::setBool(const string &location, const bool p) {
    checkBinding()
    setBool(getLocation(location), p);
}

void ShaderProgram::setInt(const string &location, const int p) {
    checkBinding()
    setInt(getLocation(location), p);
}

void ShaderProgram::setUint(const string &location, const uint p) {
    checkBinding()
    setUint(getLocation(location), p);
}

void ShaderProgram::setFloat(const string &location, const float p) {
    checkBinding()
    setFloat(getLocation(location), p);
}

void ShaderProgram::setVec3(const string &location, const glm::vec3 &p) {
    checkBinding()
    setVec3(getLocation(location), p);
}

void ShaderProgram::setVec4(const string &location, const glm::vec4 &p) {
    checkBinding()
    setVec4(getLocation(location), p);
}

void ShaderProgram::setMat3(const string &location, const glm::mat3 &p) {
    checkBinding()
    setMat3(getLocation(location), p);
}

void ShaderProgram::setMat4(const string &location, const glm::mat4 &p) {
    checkBinding()
    setMat4(getLocation(location), p);
}

ShaderProgramPtr ShaderProgram::getByName(const string &name) {
    return PublicObjectTools::getByName<ShaderProgramPtr>(name);
}

ShaderProgram* ShaderProgram::byName(const string &name) {
    return PublicObjectTools::byName<ShaderProgram>(name);
}
}
