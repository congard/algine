#ifndef ALGINE_MODULE_SHADING_SPECULAR_COOK_TORRANCE
#define ALGINE_MODULE_SHADING_SPECULAR_COOK_TORRANCE

// Based on: https://www.standardabweichung.de/code/javascript/webgl-glsl-cook-torrance-hilight
float specularCookTorrance(
    float roughnessValue,
    float fresnelReflectance,
    float IOR,
    vec3 surfacePosition,
    vec3 surfaceNormal,
    vec3 lightDirection,
    float lambertFactor
) {
    vec3 viewDirection = normalize(-surfacePosition);
    vec3 halfDirection = normalize(lightDirection + viewDirection);

    float NdotH = max(dot(surfaceNormal, halfDirection), 0.0f);
    float NdotV = max(dot(surfaceNormal, viewDirection), 0.0f);
    float VdotH = max(dot(viewDirection, halfDirection), 0.0f);

    float roughnessSquared = roughnessValue * roughnessValue;

    float NH2 = 2.0f * NdotH;
    float g1 = (NH2 * NdotV) / VdotH;
    float g2 = (NH2 * lambertFactor) / VdotH;
    float geoAtt = min(1.0f, min(g1, g2));
    float r1 = 1.0f / (4.0f * roughnessSquared * pow(NdotH, 4.0f));
    float r2 = (NdotH * NdotH - 1.0f) / (roughnessSquared * NdotH * NdotH);
    float roughness = r1 * exp(r2);
    float fresnel = pow(1.0f - VdotH, 5.0f);
    fresnel *= (1.0f - fresnelReflectance);
    fresnel += fresnelReflectance;

    float factor = (fresnel * geoAtt * roughness) / (NdotV * lambertFactor * IOR);
    return factor;
}

#endif //ALGINE_MODULE_SHADING_SPECULAR_COOK_TORRANCE
