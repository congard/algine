#ifndef ALGINE_MODULE_FRESNEL
#define ALGINE_MODULE_FRESNEL

// source: https://www.standardabweichung.de/code/javascript/webgl-glsl-fresnel-schlick-approximation
float fresnel(vec3 direction, vec3 normal) {
    vec3 halfDirection = normalize(normal + direction);

    float cosine = dot(halfDirection, direction);
    float product = max(cosine, 0.0f);
    float factor = pow(product, 5.0f);

    return factor;
}

float invertFresnel(vec3 direction, vec3 normal) {
    return 1.0f - fresnel(direction, normal);
}

#endif //ALGINE_MODULE_FRESNEL
