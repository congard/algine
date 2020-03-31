#ifndef ALGINE_MODULE_MATERIAL_H
#define ALGINE_MODULE_MATERIAL_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
namespace Module {
namespace Material {
    namespace Settings {
        constant(ExcludeDefaultProps, "ALGINE_MATERIAL_EXCLUDE_DEFAULT")
        constant(IncludeCustomProps, "ALGINE_MATERIAL_INCLUDE_CUSTOM")
    }

    namespace Vars {
        constant(AmbientTex, "material.ambient")
        constant(DiffuseTex, "material.diffuse")
        constant(SpecularTex, "material.specular")
        constant(NormalTex, "material.normal")
        constant(AmbientColor, "material.cambient")
        constant(DiffuseColor, "material.cdiffuse")
        constant(SpecularColor, "material.cspecular")
        constant(AmbientStrength, "material.ambientStrength")
        constant(DiffuseStrength, "material.diffuseStrength")
        constant(SpecularStrength, "material.specularStrength")
        constant(Shininess, "material.shininess")
    }
}
}
}

#undef constant

#endif //ALGINE_MODULE_MATERIAL_H
