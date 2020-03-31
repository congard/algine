#ifndef ALGINE_SSRSHADER_H
#define ALGINE_SSRSHADER_H

// Screen Space Reflections Shader, only fragment shader variables

#define constant(name, val) constexpr char name[] = val;

namespace algine {
namespace SSRShader {
    namespace Vars {
        constant(BaseImage, "baseImage")
        constant(NormalMap, "normalMap")
        constant(SSRValuesMap, "ssrValuesMap")
        constant(PositionMap, "positionMap")
        constant(ProjectionMatrix, "projection")
        constant(ViewMatrix, "view")
        constant(SkyColor, "skyColor")
        constant(BinarySearchCount, "binarySearchCount")
        constant(RayMarchCount, "rayMarchCount")
        constant(RayStep, "rayStep")
        constant(LLimiter, "LLimiter")
        constant(MinRayStep, "minRayStep")
    }
}

namespace Module {
namespace Material {
    namespace Vars {
        constant(ReflectionStrengthTex, "material.reflectionStrength")
        constant(JitterTex, "material.jitter")
    }
}
}
}

#undef constant

#endif //ALGINE_SSRSHADER_H
