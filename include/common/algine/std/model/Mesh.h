#ifndef ALGINE_MESH_H
#define ALGINE_MESH_H

#include <algine/types.h>
#include <algine/std/Material.h>

namespace algine {
struct Mesh {
    uint start = 0, count = 0;
    Material material;
};
}

#endif //ALGINE_MESH_H
