#include <algine/core/shader/ShaderManager.h>

#include <algine/core/JsonHelper.h>

#include <tulz/Path.h>
#include <tulz/File.h>
#include <tulz/StringUtils.h>
#include <tulz/macros.h>

#include <stdexcept>
#include <iostream>

using namespace nlohmann;
using namespace tulz;
using namespace tulz::StringUtils;
using namespace std;

#define constant(name, val) constexpr char name[] = val

namespace ShaderManager {
constant(Include, "include");
constant(Link, "link");
}

namespace Config {
constant(Name, "name");
constant(Access, "access");
constant(Type, "type");

constant(Private, "private");
constant(Public, "public");

constant(Vertex, "vertex");
constant(Fragment, "fragment");
constant(Geometry, "geometry");

constant(Source, "source");
constant(Path, "path");

constant(BaseIncludePath, "baseIncludePath");
constant(IncludePaths, "includePaths");
}

namespace algine {
ShaderManager::ShaderManager()
    : m_access(Access::Private),
      m_type(),
      m_dumperUseSources(false)
{
    // see initializer list above
}

void ShaderManager::fromFile(const string &path) {
    m_path = path;

    setBaseIncludePath(Path(path).getParentDirectory());
    fromSource(File(path, File::Read).readStr());
}

void ShaderManager::fromSource(const string &source) {
    m_source = source;
    resetGenerated();
}

void ShaderManager::setBaseIncludePath(const string &path) {
    m_baseIncludePath = path;
}

void ShaderManager::addIncludePath(const string &includePath) {
    m_includePaths.emplace_back(includePath);
}

void ShaderManager::resetGenerated() {
    m_gen = "";
}

void ShaderManager::generate() {
    constexpr char versionRegex[] = R"~([ \t]*#[ \t]*version[ \t]+[0-9]+(?:[ \t]+[a-z]+|[ \t]*)(?:\r\n|\n|$))~";

    m_gen = m_source;

    // generate definitions code
    vector<Matches> version = findRegex(m_gen, versionRegex);
    if (version.empty())
        return;

    string definitionsCode = "\n";
    for (auto & j : m_definitions)
        definitionsCode += "#define " + j.first + " " + j.second + "\n";
    m_gen.insert(version[0].pos + version[0].size, definitionsCode);

    // expand includes
    m_gen = processDirectives(m_gen, m_baseIncludePath);
}

void ShaderManager::setName(const string &name) {
    m_name = name;
}

void ShaderManager::setAccess(Access access) {
    m_access = access;
}

void ShaderManager::setType(uint type) {
    m_type = type;
}

string ShaderManager::getName() const {
    return m_name;
}

ShaderManager::Access ShaderManager::getAccess() const {
    return m_access;
}

uint ShaderManager::getType() const {
    return m_type;
}

string ShaderManager::getTemplate() {
    return m_source;
}

string ShaderManager::getGenerated() {
    return m_gen;
}

string ShaderManager::makeGenerated() {
    generate();
    return getGenerated();
}

shared_ptr<Shader> ShaderManager::createShader() {
    if (m_gen.empty())
        generate();

    shared_ptr<Shader> shader = make_shared<Shader>(m_type);
    shader->fromSource(m_gen);
    shader->setName(m_name);

    if (m_access == Access::Public) {
        auto printInfo = [&]()
        {
            if (!m_path.empty()) {
                cerr << "Path: " << m_path << "\n";
            } else if (!m_source.empty()) {
                cerr << "Source: " << m_source << "\n";
            }
        };

        if (m_name.empty()) {
            cerr << "Warning: Shader without name can't be public\n";

            printInfo();

            return shader;
        }

        if (Shader::byName(m_name) == nullptr) {
            Shader::publicShaders.emplace_back(shader);
        } else {
            cerr << "Warning: Shader with the same name was already loaded. "
                    "Access of the current shader will be set to private";

            printInfo();
        }
    }

    return shader;
}

void ShaderManager::dumperUseSources(bool use) {
    m_dumperUseSources = use;
}

void ShaderManager::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    auto loadString = [&](const string &key, string &writeTo)
    {
        if (config.contains(key))
            writeTo = config[key];
    };

    loadString(Name, m_name);
    loadString(BaseIncludePath, m_baseIncludePath);

    // load shader path or source
    if (config.contains(Source)) {
        fromSource(config[Source]);
    } else if (config.contains(Config::Path)) {
        fromFile(config[Config::Path]);
    } else {
        throw runtime_error("ShaderManager: broken file:\n" + jsonHelper.toString());
    }

    // load include paths
    if (config.contains(IncludePaths)) {
        const json &includeArray = config[IncludePaths];

        for (const auto & i : includeArray) {
            m_includePaths.emplace_back(i);
        }
    }

    // load type
    m_type = map<string, uint> {
        {Vertex, Shader::Vertex},
        {Fragment, Shader::Fragment},
        {Geometry, Shader::Geometry}
    } [config[Type]];

    // load access
    if (config.contains(Config::Access)) {
        m_access = map<string, Access> {
            {Private, Access::Private},
            {Public, Access::Public}
        } [config[Config::Access]];
    }

    ShaderDefinitionManager::import(jsonHelper);
}

JsonHelper ShaderManager::dump() {
    using namespace Config;

    json config;

    auto setString = [&](const string &key, const string &value)
    {
        if (!value.empty())
            config[key] = value;
    };

    setString(Name, m_name);
    setString(BaseIncludePath, m_baseIncludePath);

    // write source or path
    if (m_dumperUseSources) {
        setString(Source, m_source);
    } else {
        setString(Config::Path, m_path);
    }

    // write include paths
    if (!m_includePaths.empty())
        config[IncludePaths] = m_includePaths;

    // write type
    // note: Shader must keep types order
    config[Type] = vector<string> {Vertex, Fragment, Geometry} [m_type];

    // write access
    config[Config::Access] = vector<string> {Private, Public} [static_cast<uint>(m_access)];

    config.update(ShaderDefinitionManager::dump().json);

    return config;
}

void ShaderManager::importFromFile(const string &path) {
    m_confPath = path;
    Transferable::importFromFile(path);
}

// src: where to insert
// srcPos: position to start erase
// srcSize: count of symbols to erase
// data: what to insert, will be inserted in srcPos position
inline void insert(string &src, uint srcPos, uint srcSize, const string &data) {
    src = src.erase(srcPos, srcSize);
    src.insert(srcPos, data);
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

#define errFileNotFound() \
{ \
    cerr << "Err: file " << filePath << " not found\n" << matches.matches[0] << "\n\n"; \
    continue; \
}

string ShaderManager::processDirectives(const string &src, const string &baseIncludePath) {
    string result = src;
    constexpr char regex[] = R"~((\w+)[ \t]+(.+))~";

    // We process from the end because if we start from the beginning -
    // Matches::pos will be violated because of new data insertion
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
}
