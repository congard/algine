#ifndef ALGINE_COCSHADER_H
#define ALGINE_COCSHADER_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
namespace COCShader {
    namespace Settings {
        constant(Linear, "ALGINE_LINEAR_DOF")
        constant(Cinematic, "ALGINE_CINEMATIC_DOF")
    }

    namespace Vars {
        constant(PositionMap, "positionMap")

        namespace Linear {
            constant(MinSigma, "linear.minSigma")
            constant(MaxSigma, "linear.maxSigma")
            constant(FocalDepth, "linear.focalDepth")
            constant(FocalRange, "linear.focalRange")
        }

        namespace Cinematic {
            constant(PlaneInFocus, "cinematic.p")
            constant(Aperture, "cinematic.a")
            constant(ImageDistance, "cinematic.i")
        }
    }
}
}

#undef constant

#endif //ALGINE_COCSHADER_H
