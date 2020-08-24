#include <algine/core/shader/ShaderProgramManagerLegacy.h>

#include <algine/core/shader/Shader.h>
#include <algine/core/JsonHelper.h>

#include <tulz/Path.h>
#include <tulz/File.h>
#include <tulz/StringUtils.h>
#include <tulz/macros.h>

#include <iostream>

using namespace std;
using namespace tulz;
using namespace tulz::StringUtils;
using namespace nlohmann;

#define constant(name, val) constexpr char name[] = val

namespace ShaderManager {
constant(Include, "include");
constant(Link, "link");
}

namespace Config {
constant(FromSources, "fromSources");

constant(Vertex, "vertex");
constant(Fragment, "fragment");
constant(Geometry, "geometry");
constant(Common, "common");

constant(BaseIncludePaths, "baseIncludePaths");
constant(IncludePaths, "includePaths");

constant(Definitions, "definitions");
constant(Params, "params");
}

namespace algine {
void ShaderProgramManagerLegacy::fromFile(const string &vertex, const string &fragment, const string &geometry) {
    m_shaderPaths[Shader::Vertex] = vertex;
    m_shaderPaths[Shader::Fragment] = fragment;
    m_shaderPaths[Shader::Geometry] = geometry;

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

void ShaderProgramManagerLegacy::fromFile(const ShadersInfo &paths) {
    fromFile(paths.vertex, paths.fragment, paths.geometry);
}

void ShaderProgramManagerLegacy::fromSource(const string &vertex, const string &fragment, const string &geometry) {
    m_vertexTemp = vertex;
    m_fragmentTemp = fragment;
    m_geometryTemp = geometry;
    resetGenerated();
}

void ShaderProgramManagerLegacy::fromSource(const ShadersInfo &sources) {
    fromSource(sources.vertex, sources.fragment, sources.geometry);
}

inline void checkShaderType(int shaderType) {
    if (shaderType > Shader::Geometry || shaderType < -1)
        throw invalid_argument("Unknown shader type " + to_string(shaderType));
}

void ShaderProgramManagerLegacy::setBaseIncludePath(const string &path, int shaderType) {
    checkShaderType(shaderType);

    if (shaderType == -1) {
        m_baseIncludePath[Shader::Vertex] = path;
        m_baseIncludePath[Shader::Fragment] = path;
        m_baseIncludePath[Shader::Geometry] = path;
    } else
        m_baseIncludePath[shaderType] = path;
}

void ShaderProgramManagerLegacy::addIncludePath(const string &includePath) {
    m_includePaths.push_back(includePath);
}

void ShaderProgramManagerLegacy::define(const string &macro, const string &value, const int shaderType) {
    checkShaderType(shaderType);

    if (shaderType == -1) {
        m_definitions[Shader::Vertex].emplace_back(macro, value);
        m_definitions[Shader::Fragment].emplace_back(macro, value);
        m_definitions[Shader::Geometry].emplace_back(macro, value);
    } else {
        m_definitions[shaderType].emplace_back(macro, value);
    }
}

void ShaderProgramManagerLegacy::define(const string &macro, size value, int shaderType) {
    define(macro, to_string(value), shaderType);
}

void ShaderProgramManagerLegacy::removeDefinition(const uint shaderType, const string &macro, const uint type) {
    auto &shaderDefs = m_definitions[shaderType];

    switch (type) {
        case RemoveFirst:
        case RemoveAll:
            for (uint i = 0; i < static_cast<uint>(shaderDefs.size()); i++)
                if (shaderDefs[i].first == macro) {
                    shaderDefs.erase(shaderDefs.begin() + i);

                    if (shaderType == RemoveFirst)
                        return;
                }
            break;
        case RemoveLast:
            for (int i = static_cast<int>(shaderDefs.size()) - 1; i >= 0; i--)
                if (shaderDefs[i].first == macro) {
                    shaderDefs.erase(shaderDefs.begin() + i);
                    return;
                }
            break;
        default:
            cerr << "Unknown shader type " << shaderType << "\n";
            break;
    }
}

void ShaderProgramManagerLegacy::removeDefinition(const string &macro, const uint type) {
    removeDefinition(Shader::Vertex, macro, type);
    removeDefinition(Shader::Fragment, macro, type);
    removeDefinition(Shader::Geometry, macro, type);
}

void ShaderProgramManagerLegacy::resetGenerated() {
    m_vertexGen = m_vertexTemp;
    m_fragmentGen = m_fragmentTemp;
    m_geometryGen = m_geometryTemp;
}

void ShaderProgramManagerLegacy::resetDefinitions() {
    m_definitions[0].clear();
    m_definitions[1].clear();
    m_definitions[2].clear();
}

void ShaderProgramManagerLegacy::generate() {
    constexpr char versionRegex[] = R"~([ \t]*#[ \t]*version[ \t]+[0-9]+(?:[ \t]+[a-z]+|[ \t]*)(?:\r\n|\n|$))~";
    string *shaders[3] {&m_vertexGen, &m_fragmentGen, &m_geometryGen};

    for (uint i = 0; i < 3; i++) {
        // generate definitions code
        vector<Matches> version = findRegex(*shaders[i], versionRegex);
        if (version.empty())
            continue;

        string definitionsCode = "\n";
        for (auto & j : m_definitions[i])
            definitionsCode += "#define " + j.first + " " + j.second + "\n";
        shaders[i]->insert(version[0].pos + version[0].size, definitionsCode);

        // expand includes
        *shaders[i] = processDirectives(*shaders[i], m_baseIncludePath[i]);
    }
}

ShadersInfo ShaderProgramManagerLegacy::getTemplate() {
    return {
            m_vertexTemp,
            m_fragmentTemp,
            m_geometryTemp
    };
}

ShadersInfo ShaderProgramManagerLegacy::getGenerated() {
    return {
            m_vertexGen,
            m_fragmentGen,
            m_geometryGen
    };
}

ShadersInfo ShaderProgramManagerLegacy::makeGenerated() {
    generate();
    return getGenerated();
}

void ShaderProgramManagerLegacy::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // note: params are also definitions, but without value
    enum_class(DefinitionType,
               Common = -1,
               Vertex = Shader::Vertex,
               Fragment = Shader::Fragment,
               Geometry = Shader::Geometry)

    auto loadString = [](const json &config, const string &key, string &writeTo)
    {
        if (config.contains(key))
            writeTo = config[key];
    };

    auto loadDefinitions = [&](int type)
    {
        auto load = [&](int index, const string &key)
        {
            if (config.contains(Definitions) && config[Definitions].contains(key)) {
                const json &defs = config[Definitions][key];

                for (const auto &def : defs.items()) {
                    m_definitions[index].emplace_back(def.key(), def.value());
                }
            }

            if (config.contains(Params) && config[Params].contains(key)) {
                const json &params = config[Params][key];

                for (const auto &param : params) {
                    m_definitions[index].emplace_back(param, "");
                }
            }
        };

        switch (type) {
            case DefinitionType::Common:
                load(Shader::Vertex, Common);
                load(Shader::Fragment, Common);
                load(Shader::Geometry, Common);
                break;
            case DefinitionType::Vertex:
                load(type, Vertex);
                break;
            case DefinitionType::Fragment:
                load(type, Fragment);
                break;
            case DefinitionType::Geometry:
                load(type, Geometry);
                break;
            default:
                assert(0);
        }
    };

    // loading begins

    // load shader paths or sources
    {
        ShadersInfo info;

        loadString(config, Vertex, info.vertex);
        loadString(config, Fragment, info.fragment);
        loadString(config, Geometry, info.geometry);

        if (config[FromSources]) {
            fromSource(info);
        } else {
            fromFile(info);
        }
    }

    // load base include paths
    if (config.contains(BaseIncludePaths)) {
        const json &pathsCfg = config[BaseIncludePaths];

        loadString(pathsCfg, Vertex, m_baseIncludePath[Shader::Vertex]);
        loadString(pathsCfg, Fragment, m_baseIncludePath[Shader::Fragment]);
        loadString(pathsCfg, Geometry, m_baseIncludePath[Shader::Geometry]);
    }

    // load include paths
    if (config.contains(IncludePaths)) {
        const json &includeArray = config[IncludePaths];

        for (const auto & i : includeArray) {
            m_includePaths.emplace_back(i);
        }
    }

    // load definitions and params
    loadDefinitions(DefinitionType::Common);
    loadDefinitions(DefinitionType::Vertex);
    loadDefinitions(DefinitionType::Fragment);
    loadDefinitions(DefinitionType::Geometry);
}

JsonHelper ShaderProgramManagerLegacy::dump() {
    using namespace Config;

    json config;

    auto definitionIndex = [](const vector<Definition> &definitions, const Definition &definition)
    {
        for (int i = 0; i < definitions.size(); i++)
            if (definitions[i] == definition)
                return i;

        return -1; // not found
    };

    auto insertDefs = [&](const vector<Definition> &defs, const string &type)
    {
        json definitions, params;

        for (const auto & def : defs) {
            if (def.second.empty()) {
                params.emplace_back(def.first); // if value empty - it is param, not definition
            } else {
                definitions[def.first] = def.second;
            }
        }

        if (!definitions.empty())
            config[Definitions][type] = definitions;

        if (!params.empty())
            config[Params][type] = params;
    };

    auto setString = [](json &config, const string &key, const string &value)
    {
        if (!value.empty())
            config[key] = value;
    };

    config[FromSources] = m_configUseSources;

    // write sources or paths
    if (m_configUseSources) {
        setString(config, Vertex, m_vertexTemp);
        setString(config, Fragment, m_fragmentTemp);
        setString(config, Geometry, m_geometryTemp);
    } else {
        setString(config, Vertex, m_shaderPaths[Shader::Vertex]);
        setString(config, Fragment, m_shaderPaths[Shader::Fragment]);
        setString(config, Geometry, m_shaderPaths[Shader::Geometry]);
    }

    // write base include paths
    {
        json baseIncludePaths;
        setString(baseIncludePaths, Vertex, m_baseIncludePath[Shader::Vertex]);
        setString(baseIncludePaths, Fragment, m_baseIncludePath[Shader::Fragment]);
        setString(baseIncludePaths, Geometry, m_baseIncludePath[Shader::Geometry]);
        config[BaseIncludePaths] = baseIncludePaths;
    }

    // write include paths
    if (!m_includePaths.empty())
        config[IncludePaths] = m_includePaths;

    // make common, vertex, fragment and geometry definitions arrays
    {
        vector<Definition> common;
        vector<Definition> definitions[3] = {
                m_definitions[Shader::Vertex],
                m_definitions[Shader::Fragment],
                m_definitions[Shader::Geometry]
        };

        auto &vertexDefinitions = definitions[Shader::Vertex];
        auto &fragmentDefinitions = definitions[Shader::Fragment];
        auto &geometryDefinitions = definitions[Shader::Geometry];

        // finding common definitions & removing them from 'personal' arrays
        for (int i = static_cast<int>(vertexDefinitions.size()) - 1; i >= 0; i--) {
            const auto &definition = vertexDefinitions[i];

            int fragIndex = definitionIndex(fragmentDefinitions, definition);
            int geomIndex = definitionIndex(geometryDefinitions, definition);

            if (fragIndex != -1 && geomIndex != -1) {
                common.emplace_back(vertexDefinitions[i]);

                vertexDefinitions.erase(vertexDefinitions.begin() + i);
                fragmentDefinitions.erase(fragmentDefinitions.begin() + fragIndex);
                geometryDefinitions.erase(geometryDefinitions.begin() + geomIndex);
            }
        }

        // write definitions and params
        insertDefs(common, Common);
        insertDefs(vertexDefinitions, Vertex);
        insertDefs(fragmentDefinitions, Fragment);
        insertDefs(geometryDefinitions, Geometry);
    }

    return config;
}

// src: where to insert
// srcPos: position to start erase
// srcSize: count of symbols to erase
// data: what to insert, will be inserted in srcPos position
inline void insert(string &src, uint srcPos, uint srcSize, const string &data) {
    src = src.erase(srcPos, srcSize);
    src.insert(srcPos, data);
}

#define errFileNotFound() \
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

string ShaderProgramManagerLegacy::processDirectives(const string &src, const string &baseIncludePath) {
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
                    for (string &i : m_includePaths) { // i - include path
                        if (Path(Path::join(i, filePath)).exists()) {
                            filePath = Path::join(i, filePath);
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        errFileNotFound()
                    }
                }
            } else if (!Path(filePath).exists()) {
                errFileNotFound()
            }

            insert(result, matches.pos, matches.size,
                   processDirectives(File(filePath, File::Read).readStr(), Path(filePath).getParentDirectory()));
        } case_t(::ShaderManager::Link) {
            auto fileMatches = StringUtils::findRegex(matches.matches[2], R"~((.+)[ \t]+(.+))~");

            // #alp link base link
            insert(result, matches.pos, matches.size,
                   "#define " + fileMatches[0].matches[2] + " " + fileMatches[0].matches[1]);
        } default_t {
            cerr << "Unknown pragma " << pragmaName << "\n" << matches.matches[0] << "\n\n";
        }
        switch_t_end
    }

    return result;
}

#undef errFileNotFound
}
