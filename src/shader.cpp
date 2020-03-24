#include <algine/shader.h>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <algine/texture.h>
#include <tulz/File>
#include <tulz/Path>
#include <tulz/macros.h>

// constants begin
#define constant(name, val) constexpr char name[] = val;

namespace ShaderManager {
    constant(Include, "include")
    constant(Link, "link")
}
// constants end

using namespace std;
using namespace tulz;
using namespace tulz::StringUtils;

namespace algine {
void getShaderInfoLog(int shader, int type) {
    GLint infoLogSize, success;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);
    
    if (infoLogSize == 0) return;

    GLchar infoLog[infoLogSize];
    glGetShaderiv(shader, type, &success);

    if (!success) {
        glGetShaderInfoLog(shader, infoLogSize, nullptr, infoLog);
        cout << "Shader info log (for shader id " << shader << "): " << infoLog << "\n";
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
        cout << "Program info log (for program id " << program << "): " << infoLog << "\n";
    }
}

void ShaderManager::fromFile(const string &vertex, const string &fragment, const string &geometry) {
    setBaseIncludePath(Path(vertex).getParentDirectory(), ShaderType::Vertex);
    setBaseIncludePath(Path(fragment).getParentDirectory(), ShaderType::Fragment);

    if (geometry.empty()) {
        fromSource(
                File(vertex, File::Read).readStr(),
                File(fragment, File::Read).readStr());
    } else {
        setBaseIncludePath(Path(geometry).getParentDirectory(), ShaderType::Geometry);
        fromSource(
                File(vertex, File::Read).readStr(),
                File(fragment, File::Read).readStr(),
                File(geometry, File::Read).readStr());
    }
}

void ShaderManager::fromFile(const algine::ShadersData &paths) {
    fromFile(paths.vertex, paths.fragment, paths.geometry);
}

void ShaderManager::fromSource(const string &vertex, const string &fragment, const string &geometry) {
    vertexTemp = vertex;
    fragmentTemp = fragment;
    geometryTemp = geometry;
    resetGenerated();
}

void ShaderManager::fromSource(const algine::ShadersData &sources) {
    fromSource(sources.vertex, sources.fragment, sources.geometry);
}

#define _checkShaderType \
if (shaderType > ShaderType::Geometry || shaderType < -1) { \
    cerr << "Unknown shader type " << shaderType << "\n"; \
    return; \
}

void ShaderManager::setBaseIncludePath(const string &path, const int shaderType) {
    _checkShaderType

    if (shaderType == -1) {
        baseIncludePath[ShaderType::Vertex] = path;
        baseIncludePath[ShaderType::Fragment] = path;
        baseIncludePath[ShaderType::Geometry] = path;
    } else
        baseIncludePath[shaderType] = path;
}

void ShaderManager::addIncludePath(const string &includePath) {
    includePaths.push_back(includePath);
}

void ShaderManager::define(const string &macro, const string &value, const int shaderType) {
    _checkShaderType

    if (shaderType == -1) {
        definitions[ShaderType::Vertex].emplace_back(macro, value);
        definitions[ShaderType::Fragment].emplace_back(macro, value);
        definitions[ShaderType::Geometry].emplace_back(macro, value);
    } else
        definitions[shaderType].emplace_back(macro, value);
}

void ShaderManager::removeDefinition(const uint shaderType, const string &macro, const uint type) {
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
            cerr << "Unknown shader type " << shaderType << "\n";
            break;
    }
}

void ShaderManager::removeDefinition(const string &macro, const uint type) {
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
    constexpr char versionRegex[] = R"~([ \t]*#[ \t]*version[ \t]+[0-9]+(?:[ \t]+[a-z]+|[ \t]*)(?:\r\n|\n|$))~";
    string *shaders[3] {&vertexGen, &fragmentGen, &geometryGen};

    for (uint i = 0; i < 3; i++) {
        // generate definitions code
        vector<Matches> version = findRegex(*shaders[i], versionRegex);
        if (version.empty())
            continue;

        string definitionsCode = "\n";
        for (auto & j : definitions[i])
            definitionsCode += "#define " + j.first + " " + j.second + "\n";
        shaders[i]->insert(version[0].pos + version[0].size, definitionsCode);

        // expand includes
        *shaders[i] = processDirectives(*shaders[i], baseIncludePath[i]);
    }
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

ShadersData ShaderManager::makeGenerated() {
    generate();
    return getGenerated();
}

// src: where to insert
// srcPos: position to start erase
// srcSize: count of symbols to erase
// data: what to insert, will be inserted in srcPos position
#define _insert(src, srcPos, srcSize, data) \
    src = src.erase(srcPos, srcSize); \
    src.insert(srcPos, data);

#define _errFileNotFound \
{ \
    cerr << "Err: file " << filePath << " not found\n" << matches.matches[0] << "\n\n"; \
    continue; \
}

string ShaderManager::processDirectives(const string &src, const string &baseIncludePath) {
    string result = src;
    constexpr char regex[] = R"~((\w+)[ \t]+(.+))~";

    // We process from the end because if we start from the beginning -
    // Matches::pos will be violated because we insert new data
    vector<Matches> pragmas = findPragmas(result, regex);
    for (int j = static_cast<int>(pragmas.size()) - 1; j >= 0; j--) { // if pragmas empty, j can be -1
        Matches &matches = pragmas[j];
        string &pragmaName = matches.matches[1];

        switch_t(pragmaName)
        case_t(::ShaderManager::Include) {
            auto fileMatches = StringUtils::findRegex(matches.matches[2], R"~("(.+)")~"); // "file"
            string &filePath = fileMatches[0].matches[1];

            if (!Path(filePath).isAbsolute()) {
                if (Path(Path::join(baseIncludePath, filePath)).exists()) { // try to find included file in base file folder
                    filePath = Path::join(baseIncludePath, filePath);
                } else {
                    bool found = false;
                    for (string &i : includePaths) { // i - include path
                        if (Path(Path::join(i, filePath)).exists()) {
                            filePath = Path::join(i, filePath);
                            found = true;
                            break;
                        }
                    }

                    if (!found)
                        _errFileNotFound
                }
            } else if (!Path(filePath).exists())
                _errFileNotFound

            _insert(result, matches.pos, matches.size,
                    processDirectives(File(filePath, File::Read).readStr(), Path(filePath).getParentDirectory()))
        } case_t(::ShaderManager::Link) {
            auto fileMatches = StringUtils::findRegex(matches.matches[2], R"~((.+)[ \t]+(.+))~");

            // #alp link base link
            _insert(result, matches.pos, matches.size,
                    "#define " + fileMatches[0].matches[2] + " " + fileMatches[0].matches[1])
        } default_t {
            cerr << "Unknown pragma " << pragmaName << "\n" << matches.matches[0] << "\n\n";
        }
        switch_t_end
    }

    return result;
}

#undef _errFileNotFound

vector<StringUtils::Matches> ShaderManager::findPragmas(const string &src, const string &regex) {
    return StringUtils::findRegex(src,
            R"([ \t]*#[ \t]*pragma[ \t]+algine[ \t]+)" + regex +
            R"(|[ \t]*#[ \t]*alp[ \t]+)" + regex);
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
            cerr << "Unknown shader type " << type << "\n";
    }
}

void Shader::fromSource(const string &source) {
    const char *c_str = source.c_str();
    glShaderSource(id, 1, &c_str, nullptr);
    glCompileShader(id);
    getShaderInfoLog(id, GL_COMPILE_STATUS);
}

void Shader::fromFile(const string &path) {
    fromSource(File(path, File::Read).readStr());
}

ShaderProgram::ShaderProgram() {
    id = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(id);
}

void ShaderProgram::fromSource(const string &vertex, const string &fragment, const string &geometry) {
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

void ShaderProgram::fromFile(const string &vertex, const string &fragment, const string &geometry) {
    string vertexSource = vertex.empty() ? "" : File(vertex, File::Read).readStr();
    string fragmentSource = fragment.empty() ? "" : File(fragment, File::Read).readStr();
    string geometrySource = geometry.empty() ? "" : File(geometry, File::Read).readStr();
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

void ShaderProgram::loadUniformLocation(const string &name) {
    locations[name] = glGetUniformLocation(id, name.c_str());
}

void ShaderProgram::loadUniformLocations(const vector<string> &names) {
    for (const string &name : names)
        loadUniformLocation(name);
}

void ShaderProgram::loadAttribLocation(const string &name) {
    locations[name] = glGetAttribLocation(id, name.c_str());
}

void ShaderProgram::loadAttribLocations(const vector<string> &names) {
    for (const string &name : names)
        loadAttribLocation(name);
}

void ShaderProgram::loadActiveLocations() {
    int numActiveAttribs = 0;
    int numActiveUniforms = 0;
    glGetProgramInterfaceiv(id, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
    glGetProgramInterfaceiv(id, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);

    vector<char> nameData(256);
    uint properties[] = {GL_NAME_LENGTH};
    int values[getArraySize(properties)];

    for (int i = 0; i < numActiveAttribs; ++i) {
        glGetProgramResourceiv(id, GL_PROGRAM_INPUT, i, getArraySize(properties),
                               &properties[0], getArraySize(values), nullptr, &values[0]);

        nameData.resize(values[0]); //The length of the name.
        glGetProgramResourceName(id, GL_PROGRAM_INPUT, i, nameData.size(), nullptr, &nameData[0]);
        string name(&nameData[0], nameData.size() - 1);
        loadAttribLocation(name);
    }

    for (int i = 0; i < numActiveUniforms; ++i) {
        glGetProgramResourceiv(id, GL_UNIFORM, i, getArraySize(properties),
                               &properties[0], getArraySize(values), nullptr, &values[0]);

        nameData.resize(values[0]); //The length of the name.
        glGetProgramResourceName(id, GL_UNIFORM, i, nameData.size(), nullptr, &nameData[0]);
        string name(&nameData[0], nameData.size() - 1);
        loadUniformLocation(name);
    }
}

int ShaderProgram::getLocation(const string &name) {
    return locations[name];
}

void ShaderProgram::use() {
    glUseProgram(id);
}

void ShaderProgram::reset() {
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
    setBool(getLocation(location), p);
}

void ShaderProgram::setInt(const string &location, const int p) {
    setInt(getLocation(location), p);
}

void ShaderProgram::setUint(const string &location, const uint p) {
    setUint(getLocation(location), p);
}

void ShaderProgram::setFloat(const string &location, const float p) {
    setFloat(getLocation(location), p);
}

void ShaderProgram::setVec3(const string &location, const glm::vec3 &p) {
    setVec3(getLocation(location), p);
}

void ShaderProgram::setVec4(const string &location, const glm::vec4 &p) {
    setVec4(getLocation(location), p);
}

void ShaderProgram::setMat3(const string &location, const glm::mat3 &p) {
    setMat3(getLocation(location), p);
}

void ShaderProgram::setMat4(const string &location, const glm::mat4 &p) {
    setMat4(getLocation(location), p);
}
}
