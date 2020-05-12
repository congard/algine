#ifndef ALGINE_SHADER_H
#define ALGINE_SHADER_H

#include <algine/types.h>

#include <string>

namespace algine {
class Shader {
public:
    enum {
        Vertex,
        Fragment,
        Geometry
    };

    explicit Shader(uint type); // calls create(type)
    Shader();
    ~Shader();

    void create(uint type);
    void fromSource(const std::string &source);
    void fromFile(const std::string &path);

public:
    uint id = 0;
};
}

#endif //ALGINE_SHADER_H
