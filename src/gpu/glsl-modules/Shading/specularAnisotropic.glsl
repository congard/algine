#ifndef ALGINE_MODULE_SHADING_SPECULAR_ANISOTROPIC
#define ALGINE_MODULE_SHADING_SPECULAR_ANISOTROPIC

// Based on: https://www.standardabweichung.de/code/javascript/webgl-muenchen-glsl-anisotropic-hilight
float specularAnisotropic(float anisoRoughness, vec3 anisoDirection, vec3 surfacePosition, vec3 surfaceNormal, vec3 lightDirection) {
    vec3 viewDirection = normalize(-surfacePosition);
    vec3 t = cross(surfaceNormal, normalize(anisoDirection));

    float ldott = dot(lightDirection, t);
    float vdott = dot(viewDirection, t);

    float factor = pow(sin(ldott) * sin(vdott) + cos(ldott) * cos(vdott), 1.0f / anisoRoughness * 5.0f);

    return factor;
}

#endif //ALGINE_MODULE_SHADING_SPECULAR_ANISOTROPIC
