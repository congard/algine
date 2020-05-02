#ifndef ALGINE_SHADERSINFO_H
#define ALGINE_SHADERSINFO_H

#include <string>

namespace algine {
struct ShadersInfo {
    std::string
            vertex,
            fragment,
            geometry = "";
};
}

#endif //ALGINE_SHADERSINFO_H
