#ifndef ALGINE_SHADER_H
#define ALGINE_SHADER_H

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

public:
    uint id = 0;

public:
    static std::shared_ptr<Shader> getByName(const std::string &name);
    static Shader* byName(const std::string &name);

public:
    static std::vector<std::shared_ptr<Shader>> publicShaders;
};
}

#endif //ALGINE_SHADER_H
