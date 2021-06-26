#ifndef ALGINE_MODULE_SHADING_SPECULAR_PHONG
#define ALGINE_MODULE_SHADING_SPECULAR_PHONG

// Based on: https://www.standardabweichung.de/code/javascript/webgl-glsl-phong-hilight
float specularPhong(float decay, vec3 surfacePosition, vec3 surfaceNormal, vec3 lightDirection) {
    vec3 viewDirection = normalize(-surfacePosition);
    vec3 reflectionDirection = reflect(-lightDirection, surfaceNormal);

    float cosine = dot(reflectionDirection, viewDirection);
    float product = max(cosine, 0.0f);
    float factor = pow(product, decay);

    return factor;
}

#endif //ALGINE_MODULE_SHADING_SPECULAR_PHONG
