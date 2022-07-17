#include <algine/core/shader/ShaderCreator.h>
#include <algine/core/Engine.h>
#include <algine/core/log/Log.h>

#include <tulz/StringUtils.h>
#include <tulz/File.h>

#include <stdexcept>

#include "internal/PublicObjectTools.h"
#include "GLSLModules.h"
#include "GLSLShaders.h"

using namespace std;
using namespace tulz;
using namespace tulz::StringUtils;
using namespace algine::internal;

// Algine Preprocessor
namespace ALPKeywords {
constexpr auto Include = "include";
constexpr auto Link = "link";
}

constexpr auto TAG = "Algine ShaderCreator";

namespace algine {
template<typename T>
inline bool isElementExist(const vector<T> &v, const T &e) {
    return find(v.begin(), v.end(), e) != v.end();
}

template<typename T>
inline void removeElement(vector<T> &v, const T &e) {
    auto it = find(v.begin(), v.end(), e);

    if (it != v.end()) {
        v.erase(it);
    }
}

vector<string> ShaderCreator::m_globalIncludePaths;

ShaderCreator::ShaderCreator(): m_type() {}

ShaderCreator::ShaderCreator(Shader::Type type, const std::string &path)
    : m_type(type)
{
    if (string_view(path.c_str() + path.size() - 3) == "lua") {
        execute(path);
    } else {
        setPath(path);
    }
}

ShaderCreator::ShaderCreator(Shader::Type type): m_type(type) {}

ShaderCreator::ShaderCreator(const std::string &path): ShaderCreator({}, path) {}

void ShaderCreator::setType(Shader::Type type) {
    m_type = type;
}

Shader::Type ShaderCreator::getType() const {
    return m_type;
}

void ShaderCreator::setPath(const string &path) {
    m_path = path;
}

void ShaderCreator::setIncludePaths(const vector<string> &includePaths) {
    m_includePaths = includePaths;
}

void ShaderCreator::addIncludePaths(const vector<string> &includePaths) {
    for (const auto & i : includePaths) {
        addIncludePath(i);
    }
}

void ShaderCreator::addIncludePath(const string &includePath) {
    if (!isElementExist(m_includePaths, includePath)) {
        m_includePaths.emplace_back(includePath);
    }
}

void ShaderCreator::removeIncludePath(const std::string &includePath) {
    removeElement(m_includePaths, includePath);
}

const string& ShaderCreator::getPath() const {
    return m_path;
}

const vector<string>& ShaderCreator::getIncludePaths() const {
    return m_includePaths;
}

void ShaderCreator::setSource(const string &source) {
    m_source = source;
}

const string& ShaderCreator::getSource() const {
    return m_source;
}

void ShaderCreator::resetGenerated() {
    m_gen = "";
}

inline void cfgSourceImpl(ShaderCreator *self) {
    const string &source = self->getSource();
    const string &path = self->getPath();
    const string &rootDir = self->getRootDir();

    if (source.empty()) {
        if (path.empty()) {
            throw runtime_error("Source and path are empty");
        }

        constexpr char algineShaders[] = "@algine/";

        if (path.find(algineShaders) == 0) {
            string name = path.substr(strlen(algineShaders));

            for (const auto &p : GLSLShaders::shaders) {
                if (p.first == name) {
                    self->setSource(p.second);
                    return;
                }
            }

            throw runtime_error("Built-in shader '" + name + "' not found");
        } else {
            self->setSource(self->readStr(Path::join(rootDir, path)));
        }
    }
}

#define cfgSource() cfgSourceImpl(this)

void ShaderCreator::generate() {
    constexpr char versionRegex[] = R"~([ \t]*#[ \t]*version[ \t]+[0-9]+(?:[ \t]+[a-z]+|[ \t]*)(?:\r\n|\n|$))~";

    cfgSource();

    m_gen = m_source;

    // generate definitions code
    {
        vector<Matches> version = findRegex(m_gen, versionRegex);

        uint versionHeaderOffset;

        if (version.empty()) {
            string versionHeader = "#version " + to_string(Engine::getAPIVersion()) + " ";

            if (Engine::getGraphicsAPI() == Engine::GraphicsAPI::Core) {
                versionHeader.append("core");
            } else {
                versionHeader.append("es");
            }

            versionHeader.append("\n");

            versionHeaderOffset = versionHeader.size();

            m_gen.insert(0, versionHeader);
        } else {
            versionHeaderOffset = version[0].pos + version[0].size;
        }

        string definitionsCode = "\n";

        for (auto &j : m_definitions) {
            definitionsCode += "#define " + j.first + " " + j.second + "\n";
        }

        m_gen.insert(versionHeaderOffset, definitionsCode);
    }

    // expand includes
    // base include path (path where file is located)
    // it is working directory (if specified)
    m_gen = processDirectives(m_gen, !m_path.empty() ? Path(m_path).getParentDirectory() : Path(getRootDir()));
}

const string& ShaderCreator::getGenerated() const {
    return m_gen;
}

const string& ShaderCreator::makeGenerated() {
    generate();
    return getGenerated();
}

ShaderPtr ShaderCreator::get() {
    return PublicObjectTools::getPtr<ShaderPtr>(this);
}

ShaderPtr ShaderCreator::create() {
    generate();

    ShaderPtr shader = make_shared<Shader>(m_type);
    shader->fromSource(m_gen);
    shader->setName(m_name);

    PublicObjectTools::postCreateAccessOp("Shader", this, shader);

    return shader;
}

void ShaderCreator::registerLuaUsertype(Lua *lua, sol::global_table *tenv) {
    auto &env = getEnv(lua, tenv);

    if (isRegistered(env, "ShaderCreator"))
        return;

    lua->registerUsertype<Shader, Creator, ShaderDefinitionGenerator>(tenv);

    auto ctors = sol::constructors<ShaderCreator(), ShaderCreator(Shader::Type),
        ShaderCreator(Shader::Type, const string&), ShaderCreator(const string&)>();
    auto usertype = env.new_usertype<ShaderCreator>(
            "ShaderCreator",
            sol::meta_function::construct, ctors,
            sol::call_constructor, ctors,
            sol::base_classes, sol::bases<Scriptable, IOProvider, Creator, ShaderDefinitionGenerator>());

    Lua::new_property(usertype, "type", &ShaderCreator::getType, &ShaderCreator::setType);
    Lua::new_property(usertype, "path", &ShaderCreator::getPath, &ShaderCreator::setPath);
    Lua::new_property(usertype, "includePaths",
        &ShaderCreator::getIncludePaths,
        [](ShaderCreator &self, vector<string> paths) { self.setIncludePaths(paths); });
    Lua::new_property(usertype, "source", &ShaderCreator::getSource, &ShaderCreator::setSource);

    usertype["addIncludePaths"] = [](ShaderCreator &self, vector<string> paths) { self.addIncludePaths(paths); };
    usertype["addIncludePath"] = &ShaderCreator::addIncludePath;
    usertype["removeIncludePath"] = &ShaderCreator::removeIncludePath;
    usertype["resetGenerated"] = &ShaderCreator::resetGenerated;
    usertype["generate"] = &ShaderCreator::generate;
    usertype["getGenerated"] = &ShaderCreator::getGenerated;
    usertype["makeGenerated"] = &ShaderCreator::makeGenerated;
    usertype["get"] = &ShaderCreator::get;
    usertype["create"] = &ShaderCreator::create;

    // static methods
    usertype["getGlobalIncludePaths"] = &ShaderCreator::getGlobalIncludePaths;
    usertype["setGlobalIncludePaths"] = [](std::vector<std::string> paths) { setGlobalIncludePaths(paths); };
    usertype["addGlobalIncludePath"] = &ShaderCreator::addGlobalIncludePath;
    usertype["removeGlobalIncludePath"] = &ShaderCreator::removeGlobalIncludePath;
}

void ShaderCreator::exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) {
    exec_t<ShaderCreator>(s, path, lua, tenv);
}

void ShaderCreator::setGlobalIncludePaths(const vector<string> &includePaths) {
    m_globalIncludePaths = includePaths;
}

void ShaderCreator::addGlobalIncludePaths(const vector<string> &includePaths) {
    for (const auto & i : includePaths) {
        addGlobalIncludePath(i);
    }
}

void ShaderCreator::addGlobalIncludePath(const string &includePath) {
    if (!isElementExist(m_globalIncludePaths, includePath)) {
        m_globalIncludePaths.emplace_back(includePath);
    }
}

void ShaderCreator::removeGlobalIncludePath(const string &includePath) {
    removeElement(m_globalIncludePaths, includePath);
}

vector<string>& ShaderCreator::getGlobalIncludePaths() {
    return m_globalIncludePaths;
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

            if (matches.empty())
                return matches;

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

string ShaderCreator::processDirectives(const string &src, const Path &baseIncludePath) {
    string result = src;
    constexpr char regex[] = R"~((\w+)[ \t]+(.+))~";

    using namespace ALPKeywords;

    // We process from the end because if we start from the beginning -
    // Matches::pos will be violated because of new data insertion
    vector<Matches> pragmas = findPragmas(result, regex, findComments(src));

    for (int j = static_cast<int>(pragmas.size()) - 1; j >= 0; j--) { // if pragmas empty, j can be -1
        Matches &matches = pragmas[j];
        string &pragmaName = matches.matches[1];

        auto pragmaIs = [&](const string &name) {
            return pragmaName == name;
        };

        if (pragmaIs(Include)) {
            // check for inclusion of built-in files
            vector<Matches> fileMatches = StringUtils::findRegex(matches.matches[2], R"~(<(.+)>)~"); // <file>;

            if (!fileMatches.empty()) {
                string &path = fileMatches[0].matches[1];

                for (const auto &p : GLSLModules::modules) {
                    if (path == p.first) {
                        insert(result, matches.pos, matches.size, processDirectives(p.second, {}));
                        goto success;
                    }
                }

                throw runtime_error("Unknown built-in header: " + path);

                success:
                continue;
            }

            // otherwise process as inclusion of user file
            fileMatches = StringUtils::findRegex(matches.matches[2], R"~("(.+)")~"); // "file"
            Path filePath(fileMatches[0].matches[1]);

            auto fileNotFoundError = [&]() {
                Log::error(TAG) << "ShaderCreator: Error: file " << filePath.toString() << " not found\n" << matches.matches[0];
            };

            auto exists = [&](const Path &p) {
                return io()->exists(p.toString());
            };

            if (!filePath.isAbsolute()) {
                if (exists(Path::join(baseIncludePath, filePath))) { // try to find included file in base file folder
                    filePath = Path::join(baseIncludePath, filePath);
                } else {
                    bool found = false;

                    auto findIncludePath = [&](const vector<string> &includePaths) {
                        for (const auto &i : includePaths) { // i - include path
                            Path includePath(i);

                            if (exists(Path::join(includePath, filePath))) {
                                filePath = Path::join(includePath, filePath);
                                found = true;
                                break;
                            }
                        }
                    };

                    // try to find include path in the own array
                    findIncludePath(m_includePaths);

                    // otherwise try to find include path in the global array
                    if (!found)
                        findIncludePath(m_globalIncludePaths);

                    if (!found) {
                        fileNotFoundError();
                        continue;
                    }
                }
            } else if (!exists(Path(filePath))) {
                fileNotFoundError();
                continue;
            }

            insert(result, matches.pos, matches.size,
                   processDirectives(readStr(filePath.toString()), filePath.getParentDirectory()));
        } else if (pragmaIs(Link)) {
            auto fileMatches = StringUtils::findRegex(matches.matches[2], R"~((.+)[ \t]+(.+))~");

            // #alp link base link
            insert(result, matches.pos, matches.size,
                   "#define " + fileMatches[0].matches[2] + " " + fileMatches[0].matches[1]);
        } else {
            Log::error(TAG) << "Unknown pragma " << pragmaName << "\n" << matches.matches[0];
        }
    }

    return result;
}
}
