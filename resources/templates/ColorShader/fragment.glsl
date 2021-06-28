#version 400 core

uniform sampler2D ambient;
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D normal;
uniform sampler2D reflectionStrength;
uniform sampler2D jitter;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;

uniform mat4 viewMatrix;

in mat3 matTBN;
in vec3 worldPosition; // fragment pos in world space
in vec3 viewPosition;
in vec2 texCoord;

// output colors
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 normalBuffer;
layout(location = 2) out vec3 positionBuffer;
layout(location = 3) out vec2 ssrValuesBuffer;

#alp include <NormalMapping.fs>
#alp include <Shading/diffuseLambert>
#alp include <Shading/specularBlinnPhong>
#alp include <Shading/attenuation>
#alp include <Shading/dirLightShadow>
#alp include <Shading/pointLightShadow>
#alp include "modules/Lighting.glsl"

struct LightingResult {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 lightDir;
};

vec3 viewNormal;

vec3 ambientResult = vec3(0.0f);
vec3 diffuseResult = vec3(0.0f);
vec3 specularResult = vec3(0.0f);

LightingResult calculateBaseLighting(vec3 pos, vec3 color, float kc, float kl, float kq) {
    vec3 lampEyePos = vec3(viewMatrix * vec4(pos, 1.0));

    LightingResult lighting;
    lighting.lightDir = normalize(lampEyePos - viewPosition);

    float attenuation = attenuation(length(lampEyePos - viewPosition), kc, kl, kq);

    // ambient
    lighting.ambient = ambientStrength * color * attenuation;

    // diffuse
    float diff = diffuseLambert(viewNormal, lighting.lightDir);
    lighting.diffuse = diffuseStrength * diff * color * attenuation;

    // specular
    float spec = specularBlinnPhong(shininess, viewPosition, viewNormal, lighting.lightDir);
    lighting.specular = specularStrength * spec * color * attenuation;

    return lighting;
}

void calculatePointLighting() {
    for (uint i = 0; i < pointLightsCount; i++) {
        LightingResult lighting = calculateBaseLighting(pointLights[i].pos, pointLights[i].color, pointLights[i].kc, pointLights[i].kl, pointLights[i].kq);

        float shadow = pointLightSoftShadow(
            worldPosition,
            pointLights[i].pos,
            length(viewPosition),
            diskRadius_k,
            diskRadius_min,
            pointLights[i].bias,
            pointLights[i].far,
            pointLightShadowMaps[i]
        ) * shadowOpacity;

        ambientResult += lighting.ambient;
        diffuseResult += lighting.diffuse * (1 - shadow);
        specularResult += lighting.specular * (1 - shadow);
    }
}

void calculateDirLighting() {
    for (uint i = 0; i < dirLightsCount; i++) {
        LightingResult lighting = calculateBaseLighting(dirLights[i].pos, dirLights[i].color, dirLights[i].kc, dirLights[i].kl, dirLights[i].kq);

        float shadow = dirLightSoftShadow(
            dirLights[i].lightMatrix,
            worldPosition,
            viewNormal,
            lighting.lightDir,
            dirLights[i].minBias,
            dirLights[i].maxBias,
            dirLightShadowMaps[i]
        ) * shadowOpacity;

        ambientResult += lighting.ambient;
        diffuseResult += lighting.diffuse * (1 - shadow);
        specularResult += lighting.specular * (1 - shadow);
    }
}

void main() {
	viewNormal = getNormal(normal, texCoord, matTBN);

	calculateDirLighting();
	calculatePointLighting();

	fragColor =
			vec4(ambientResult, 1.0f) * texture(ambient, texCoord) +
			vec4(diffuseResult, 1.0f) * texture(diffuse, texCoord) +
			vec4(specularResult, 1.0f) * texture(specular, texCoord);

	normalBuffer = viewNormal;
	positionBuffer = viewPosition;
	ssrValuesBuffer.r = texture(reflectionStrength, texCoord).r;
	ssrValuesBuffer.g = texture(jitter, texCoord).r;
}
