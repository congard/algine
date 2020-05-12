#include <algine/core/shader/ShaderManager.h>

#include <algine/core/shader/Shader.h>

#include <tulz/Path>
#include <tulz/File>
#include <tulz/StringUtils>
#include <tulz/macros.h>
#include <iostream>

using namespace std;
using namespace tulz;
using namespace tulz::StringUtils;

#define constant(name, val) constexpr char name[] = val;
namespace ShaderManager {
    constant(Include, "include")
    constant(Link, "link")
}

namespace algine {
void ShaderManager::fromFile(const string &vertex, const string &fragment, const string &geometry) {
    setBaseIncludePath(Path(vertex).getParentDirectory(), Shader::Vertex);
    setBaseIncludePath(Path(fragment).getParentDirectory(), Shader::Fragment);

    if (geometry.empty()) {
        fromSource(
                File(vertex, File::Read).readStr(),
                File(fragment, File::Read).readStr());
    } else {
        setBaseIncludePath(Path(geometry).getParentDirectory(), Shader::Geometry);
        fromSource(
                File(vertex, File::Read).readStr(),
                File(fragment, File::Read).readStr(),
                File(geometry, File::Read).readStr());
    }
}

void ShaderManager::fromFile(const ShadersInfo &paths) {
    fromFile(paths.vertex, paths.fragment, paths.geometry);
}

void ShaderManager::fromSource(const string &vertex, const string &fragment, const string &geometry) {
    vertexTemp = vertex;
    fragmentTemp = fragment;
    geometryTemp = geometry;
    resetGenerated();
}

void ShaderManager::fromSource(const ShadersInfo &sources) {
    fromSource(sources.vertex, sources.fragment, sources.geometry);
}

#define _checkShaderType \
if (shaderType > Shader::Geometry || shaderType < -1) { \
    cerr << "Unknown shader type " << shaderType << "\n"; \
    return; \
}

void ShaderManager::setBaseIncludePath(const string &path, const int shaderType) {
    _checkShaderType

    if (shaderType == -1) {
        baseIncludePath[Shader::Vertex] = path;
        baseIncludePath[Shader::Fragment] = path;
        baseIncludePath[Shader::Geometry] = path;
    } else
        baseIncludePath[shaderType] = path;
}

void ShaderManager::addIncludePath(const string &includePath) {
    includePaths.push_back(includePath);
}

void ShaderManager::define(const string &macro, const string &value, const int shaderType) {
    _checkShaderType

    if (shaderType == -1) {
        definitions[Shader::Vertex].emplace_back(macro, value);
        definitions[Shader::Fragment].emplace_back(macro, value);
        definitions[Shader::Geometry].emplace_back(macro, value);
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
    removeDefinition(Shader::Vertex, macro, type);
    removeDefinition(Shader::Fragment, macro, type);
    removeDefinition(Shader::Geometry, macro, type);
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

ShadersInfo ShaderManager::getTemplate() {
    return ShadersInfo {
        vertexTemp,
        fragmentTemp,
        geometryTemp
    };
}

ShadersInfo ShaderManager::getGenerated() {
    return ShadersInfo {
        vertexGen,
        fragmentGen,
        geometryGen
    };
}

ShadersInfo ShaderManager::makeGenerated() {
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

inline vector<Matches> findPragmas(const string &src, const string &regex, const vector<pair<uint, uint>> &excludes) {
    auto matches = StringUtils::findRegex(src,
            R"([ \t]*#[ \t]*pragma[ \t]+algine[ \t]+)" + regex +
            R"(|[ \t]*#[ \t]*alp[ \t]+)" + regex);

    for (int i = static_cast<int>(matches.size()) - 1; i >= 0; i--) {
        for (auto &exclude : excludes) {
            if (matches[i].pos > exclude.first && matches[i].pos + matches[i].size <= exclude.first + exclude.second) {
                matches.erase(matches.begin() + i);
            }
        }
    }

    return matches;
}

inline vector<pair<uint, uint>> findComments(const string &src) {
    constexpr char regex[] = R"(//.*|/\*(?:.|\n|\r\n)*?\*/)"; // line and block comments
    auto matches = StringUtils::findRegex(src, regex);
    vector<pair<uint, uint>> result;
    result.reserve(matches.size());

    for (const auto &match : matches)
        result.emplace_back(match.pos, match.size);
    return result;
}

string ShaderManager::processDirectives(const string &src, const string &baseIncludePath) {
    string result = src;
    constexpr char regex[] = R"~((\w+)[ \t]+(.+))~";

    // We process from the end because if we start from the beginning -
    // Matches::pos will be violated because we insert new data
    vector<Matches> pragmas = findPragmas(result, regex, findComments(src));
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
}
