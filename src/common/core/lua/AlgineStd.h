#ifndef ALGINE_ALGINESTD_H
#define ALGINE_ALGINESTD_H

#include <algine/core/lua/Lua.h>

#ifndef ALGINE_CORE_ONLY
#include <algine/std/model/ShapeCreator.h>
#include <algine/std/model/Shape.h>
#include <algine/std/model/ModelCreator.h>
#include <algine/std/model/Model.h>
#include <algine/std/model/InputLayoutShapeLocations.h>
#include <algine/std/rotator/Rotator.h>
#include <algine/std/rotator/EulerRotator.h>
#include <algine/std/rotator/FreeRotator.h>

namespace algine {
using AlgineStd = Lua::TypeList<
        ShapeCreator, Shape,
        ModelCreator, Model,
        InputLayoutShapeLocations,
        Rotator, EulerRotator, FreeRotator>;
}
#else
namespace algine {
using AlgineStd = Lua::TypeList<>;
}
#endif

#endif //ALGINE_ALGINESTD_H
