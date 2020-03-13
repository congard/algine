#ifndef ALGINE_CUBEMAPSHADER_H
#define ALGINE_CUBEMAPSHADER_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
    namespace CubemapShader {
        namespace Settings {
            constant(OutputType, "vecout")

            constant(CubePositions, "ALGINE_CUBE_POSITIONS")
            constant(SpherePositions, "ALGINE_SPHERE_POSITIONS")
            constant(ColorOut, "ALGINE_CUBEMAP_COLOR_OUT_COLOR_COMPONENT")
            constant(PosOut, "ALGINE_POS_OUT_COLOR_COMPONENT")
        }

        namespace Vars {
            constant(InPos, "inPos")
            constant(TransformationMatrix, "transform")

            constant(ViewMatrix, "view")
            constant(CubemapTex, "cubemap")
            constant(Color, "color")
            constant(PosScaling, "scaling")
        }
    }
}

#undef constant

#endif //ALGINE_CUBEMAPSHADER_H
