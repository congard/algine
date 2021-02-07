#ifndef ALGINE_QUADSHADER_H
#define ALGINE_QUADSHADER_H

// only vertex shader variables

#define constant(name, val) constexpr char name[] = val;

namespace algine {
    namespace QuadShader {
        namespace Vars {
            constant(InPos, "inPos")
        }
    }
}

#undef constant

#endif //ALGINE_QUADSHADER_H
