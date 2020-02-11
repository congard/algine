#ifndef ALGINE_AMTL_H
#define ALGINE_AMTL_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
    namespace constants {
        namespace AMTL {
            constant(Name, "name")
            constant(Reflection, "reflection")
            constant(Jitter, "jitter")
            constant(AmbientStrength, "ambientStrength")
            constant(DiffuseStrength, "diffuseStrength")
            constant(SpecularStrength, "specularStrength")
            constant(Shininess, "shininess")

            namespace Texture {
                constant(Path, "path")
                constant(Ambient, "ambientTex")
                constant(Diffuse, "diffuseTex")
                constant(Specular, "specularTex")
                constant(Normal, "normalTex")
                constant(Reflection, "reflectionTex")
                constant(Jitter, "jitterTex")
                constant(SharedLevel, "sharedLevel")
                constant(Unique, "unique")
                constant(ModelShared, "modelShared")
                constant(Shared, "shared")

                namespace Params {
                    constant(Params, "params")

                    namespace Keys {
                        constant(WrapU, "wrapU")
                        constant(WrapV, "wrapV")
                        constant(MinFilter, "minFilter")
                        constant(MagFilter, "magFilter")
                    }

                    namespace Values {
                        constant(Repeat, "repeat")
                        constant(ClampToEdge, "clampToEdge")
                        constant(ClampToBorder, "clampToBorder")
                        constant(MirroredRepeat, "mirroredRepeat")
                        constant(MirrorClampToEdge, "mirrorClampToEdge")
                        constant(Nearest, "nearest")
                        constant(Linear, "linear")
                    }
                }
            }
        }
    }
}

#undef constant
#endif //ALGINE_AMTL_H
