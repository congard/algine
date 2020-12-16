#ifndef ALGINE_SHAPECONFIGTOOLS_H
#define ALGINE_SHAPECONFIGTOOLS_H

#include <algine/std/model/ShapeManager.h>

#include <stdexcept>

#include "../../core/ConfigStrings.h"

namespace algine::Config {
constant(Triangulate, "triangulate");
constant(SortByPolygonType, "sortByPolygonType");
constant(CalcTangentSpace, "calcTangentSpace");
constant(JoinIdenticalVertices, "joinIdenticalVertices");
constant(InverseNormals, "inverseNormals");
constant(DisableBones, "disableBones");

constant(InputLayoutLocations, "inputLayoutLocations");
constant(BonesPerVertex, "bonesPerVertex");
constant(AMTL, "amtl");

#define param_str(name) if (param == ShapeManager::Param::name) return name

inline string paramToString(ShapeManager::Param param) {
    param_str(Triangulate);
    param_str(SortByPolygonType);
    param_str(CalcTangentSpace);
    param_str(JoinIdenticalVertices);
    param_str(InverseNormals);
    param_str(DisableBones);

    throw runtime_error("Unsupported param " + to_string(static_cast<int>(param)));
}

#undef param_str

#define param(name) if (str == name) return ShapeManager::Param::name

inline ShapeManager::Param stringToParam(const string &str) {
    param(Triangulate);
    param(SortByPolygonType);
    param(CalcTangentSpace);
    param(JoinIdenticalVertices);
    param(InverseNormals);
    param(DisableBones);

    throw runtime_error("Unsupported param '" + str + "'");
}

#undef param
}

#endif //ALGINE_SHAPECONFIGTOOLS_H
