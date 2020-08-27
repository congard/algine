#ifndef ALGINE_SHADER_H
#define ALGINE_SHADER_H

#include <algine/core/shader/ShaderPtr.h>
#include <algine/core/Object.h>
#include <algine/types.h>

#include <string>
#include <vector>
#include <memory>

namespace algine {
class Shader: public Object {
public:
    enum {
        Vertex,
        Fragment,
        Geometry
    };

public:
    explicit Shader(uint type); // calls create(type)
    Shader();
    ~Shader();

    void create(uint type);
    void fromSource(const std::string &source);
    void fromFile(const std::string &path);

    uint getId() const;

public:
    static ShaderPtr getByName(const std::string &name);
    static Shader* byName(const std::string &name);

public:
    static std::vector<ShaderPtr> publicShaders;

private:
    uint m_id = 0;
};
}

#endif //ALGINE_SHADER_H
