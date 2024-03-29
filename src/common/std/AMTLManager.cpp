#include <algine/std/AMTLManager.h>
#include <algine/core/texture/Texture2D.h>
#include <algine/core/scene/GlobalScene.h>

#include <tulz/Path.h>

using namespace std;
using namespace tulz;

namespace algine {
AMTLManager::AMTLManager()
    : m_parent(GlobalScene::getInstance()) {}

void AMTLManager::setParent(Object *parent) {
    m_parent = parent;
}

Object* AMTLManager::getParent() const {
    return m_parent;
}

void AMTLManager::setMaterials(const vector<AMTLMaterialManager> &materials) {
    m_materials = materials;
}

AMTLMaterialManager& AMTLManager::addMaterial(const AMTLMaterialManager &material, const string &name) {
    if (name.empty()) {
        return m_materials.emplace_back(material);
    } else {
        auto customNamedMaterial = material;
        customNamedMaterial.setName(name);
        return m_materials.emplace_back(customNamedMaterial);
    }
}

const vector<AMTLMaterialManager>& AMTLManager::getMaterials() {
    return m_materials;
}

inline int getMaterialIndex(string_view name, const vector<AMTLMaterialManager> &materials) {
    for (int i = 0; i < materials.size(); i++) {
        if (materials[i].getName() == name) {
            return i;
        }
    }

    return -1;
}

AMTLMaterialManager& AMTLManager::getMaterial(string_view name) {
    int index = getMaterialIndex(name, m_materials);

    if (index != -1) {
        m_materials[index].setIOSystem(io());
        return m_materials[index];
    }

    throw runtime_error("AMTLMaterial '" + string(name) + "' does not found");
}

bool AMTLManager::isMaterialExists(string_view name) const {
    return getMaterialIndex(name, m_materials) != -1;
}

void AMTLManager::loadFile(std::string_view path_view, Lua *lua) {
    string path = path_view.data();

    if (getRootDir().empty()) {
        Path p(path);
        setRootDir(p.getParentDirectory().toString());
        path = p.getPathName();
    }

    load(path, true, lua);
}

void AMTLManager::loadScript(std::string_view script, Lua *lua) {
    load(script, false, lua);
}

void AMTLManager::load(std::string_view s, bool path, Lua *lua) {
    AMTLMaterialManager *material {nullptr};
    std::map<uint, uint> params = Texture2D::defaultParams();

    auto checkMaterial = [&material]() {
        if (!material) {
            throw runtime_error("Error: material name was not specified");
        }
    };

    lua = lua ? lua : &Lua::getDefault();

    Lua::Locker locker(lua);

    auto &state = lua->state();
    auto env = lua->createEnvironment(lua->getGlobalEnvironment());

    lua->registerUsertype<Texture, Texture2DBuilder>(&env);

    class Name {
    public:
        explicit Name(string_view name): m_name(name) {};
        std::string m_name;
    };

    auto nameCtors = sol::constructors<Name(string_view)>();
    env.new_usertype<Name>("Name", sol::meta_function::construct, nameCtors, sol::call_constructor, nameCtors);

    env["material"] = [&](string_view name) {
        if (!isMaterialExists(name)) {
            material = &addMaterial(AMTLMaterialManager(name));
        } else {
            material = &getMaterial(name);
        }
    };

    env["texParams"] = [&](std::map<uint, uint> p) {
        std::swap(params, p);
    };

    env["set"] = sol::overload(
        [&](const string &prop, const string &path, sol::optional<std::string> name) {
            checkMaterial();

            Texture2DBuilder builder;
            builder.setIOSystem(io());
            builder.setRootDir(getRootDir());
            builder.setDefaultParams(params);
            builder.setParent(m_parent);

            if (name.has_value())
                builder.setName(name.value());

            if (path.size() >= 4 && string_view(path.c_str() + path.size() - 4) == ".lua") {
                builder.execute(path, lua, &env);
            } else {
                builder.setPath(path);
            }

            material->setTexture(prop, builder);
        },
        [&](const string &prop, const Name &name) {
            checkMaterial();
            material->setTextureName(prop, name.m_name);
        },
        [&](const string &prop, Texture2DBuilder &builder) {
            checkMaterial();
            builder.setDefaultParams(params);
            material->setTexture(prop, builder);
        },
        [&](const string &prop, float n) {
            checkMaterial();
            material->setFloat(prop, n);
        }
    );

    char buffer[128];

    for (auto &func : {"ambient", "diffuse", "specular", "normal", "reflection", "jitter",
                       "shininess", "ambientStrength", "diffuseStrength", "specularStrength"}) {
        sprintf(buffer, "function %s(...) set('%s', ...) end", func, func);
        state.script(buffer, env);
    }

    if (path) {
        state.script(readStr(Path::join(getRootDir(), string(s))), env);
    } else {
        state.script(s, env);
    }

    env.abandon();
}

void AMTLManager::exec(const std::string &s, bool path, Lua *lua, sol::environment *tenv) {
    exec_t<AMTLManager>(s, path, lua, tenv);
}
}
