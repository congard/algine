#ifndef ALGINE_MODULE_SHADING_DIFFUSE_LAMBERT
#define ALGINE_MODULE_SHADING_DIFFUSE_LAMBERT

// Based on: https://www.standardabweichung.de/code/javascript/webgl-glsl-lambert-diffuse-shading
float diffuseLambert(vec3 surfaceNormal, vec3 lightDirection) {
    float cosine = dot(surfaceNormal, lightDirection);
    float product = max(cosine, 0.0f);

    return product;
}

#endif //ALGINE_MODULE_SHADING_DIFFUSE_LAMBERT
