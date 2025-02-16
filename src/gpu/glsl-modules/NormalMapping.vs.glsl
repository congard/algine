#ifndef ALGINE_MODULE_NORMALMAPPING_VS
#define ALGINE_MODULE_NORMALMAPPING_VS

mat3 getTBNMatrix(in mat4 modelViewMatrix, in vec3 tangent, in vec3 bitangent, in vec3 normal) {
    return mat3(
        normalize(vec3(modelViewMatrix * vec4(tangent, 0.0))),
        normalize(vec3(modelViewMatrix * vec4(bitangent, 0.0))),
        normalize(vec3(modelViewMatrix * vec4(normal, 0.0)))
    );
}

#endif //ALGINE_MODULE_NORMALMAPPING_VS
