#ifndef ALGINE_SHADER_H
#define ALGINE_SHADER_H

#include <algine/core/shader/ShaderPtr.h>
#include <algine/core/Object.h>
#include <algine/templates.h>
#include <algine/types.h>

#include <string>
#include <vector>
#include <memory>

namespace algine {
class Shader: public Object {
public:
    enum class Type {
        Vertex,
        Fragment,
        Geometry
    };

public:
    explicit Shader(Type type); // calls create(type)
    Shader();
    ~Shader();

    void create(Type type);
    void fromSource(const std::string &source);
    void fromFile(const std::string &path);

    uint getId() const;

    static void registerLuaUsertype(Lua *lua);

public:
    static ShaderPtr getByName(std::string_view name);
    static Shader* byName(std::string_view name);

public:
    static std::vector<ShaderPtr> publicObjects;

private:
    uint m_id = 0;
};
}

#endif //ALGINE_SHADER_H
