/**
 * Normal mapping vertex module
 * Include it in vertex shader
 */

#ifndef ALGINE_MODULE_NORMALMAPPING_VERT_GLSL
#define ALGINE_MODULE_NORMALMAPPING_VERT_GLSL

uniform bool u_NormalMapping;
out mat3 v_TBN;

#define isNormalMappingEnabled() u_NormalMapping

mat3 getTBN(mat4 modelViewMatrix, vec3 tangent, vec3 bitangent, vec3 normal) {
    return mat3(
        normalize(vec3(modelViewMatrix * vec4(tangent, 0.0))),
        normalize(vec3(modelViewMatrix * vec4(bitangent, 0.0))),
        normalize(vec3(modelViewMatrix * vec4(normal, 0.0)))
    );
}

void passTBN(mat3 matTBN) {
    v_TBN = matTBN;
}

void passTBN(mat4 modelViewMatrix, vec3 tangent, vec3 bitangent, vec3 normal) {
    passTBN(getTBN(modelViewMatrix, tangent, bitangent, normal));
}

#endif // ALGINE_MODULE_NORMALMAPPING_VERT_GLSL