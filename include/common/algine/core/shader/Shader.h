#ifndef ALGINE_SHADER_H
#define ALGINE_SHADER_H

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
    explicit Shader(Type type, Object *parent = defaultParent()); // calls create(type)
    explicit Shader(Object *parent = defaultParent());
    ~Shader() override;

    void create(Type type);
    void fromSource(const std::string &source);
    void fromFile(const std::string &path);

    uint getId() const;

private:
    uint m_id = 0;
};
}

#endif //ALGINE_SHADER_H
