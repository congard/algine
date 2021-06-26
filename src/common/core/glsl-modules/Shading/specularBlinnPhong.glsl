#ifndef ALGINE_MODULE_SHADING_SPECULAR_BLINN_PHONG
#define ALGINE_MODULE_SHADING_SPECULAR_BLINN_PHONG

// Based on: https://www.standardabweichung.de/code/javascript/webgl-glsl-blinn-hilight
float specularBlinnPhong(float decay, vec3 surfacePosition, vec3 surfaceNormal, vec3 lightDirection) {
    vec3 viewDirection = normalize(-surfacePosition);
    vec3 halfDirection = normalize(lightDirection + viewDirection);

    float cosine = dot(halfDirection, surfaceNormal);
    float product = max(cosine, 0.0f);
    float factor = pow(product, decay * 4.0f);

    return factor;
}

#endif //ALGINE_MODULE_SHADING_SPECULAR_BLINN_PHONG
