#ifndef ALGINE_SHADOWSHADER_H
#define ALGINE_SHADOWSHADER_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
    namespace ShadowShader {
        namespace Settings {
            constant(BoneSystem, "ALGINE_BONE_SYSTEM")
            constant(PointLightShadowMapping, "ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING")
            constant(DirLightShadowMapping, "ALGINE_SHADOW_MAPPING_TYPE_DIR_LIGHTING")
        }

        namespace Vars {
            constant(InPos, "a_Position")
            constant(TransformationMatrix, "transformationMatrix")

            namespace PointLight {
                constant(ShadowMatrices, "shadowMatrices[0]") // from geometry shader
                constant(Pos, "lightPos")
                constant(FarPlane, "farPlane")
            }
        }
    }
}

#undef constant

#endif //ALGINE_SHADOWSHADER_H
