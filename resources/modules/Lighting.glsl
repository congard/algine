/**
 * Lighting module
 * Include it in fragment shader
 * Necessary variables:

    mat4 lightingViewMatrix;
    vec3 lightingCameraPos;
    vec3 lightingWorldPos; // position for this fragment in world space, need for shadow mapping

 * You must link these variables with yours
 */

// TODO: split this file to the modules

#ifndef ALGINE_MODULE_LIGHTING_GLSL
#define ALGINE_MODULE_LIGHTING_GLSL

#alp include <Shading/diffuseLambert>
#alp include <Shading/specularBlinnPhong>
#alp include <Shading/attenuation>
#alp include <Shading/dirLightShadow>
#alp include <Shading/pointLightShadow>

struct DirLight {
	float kc; // constant term
	float kl; // linear term
	float kq; // quadratic term
	float minBias, maxBias;
	vec3 pos; // in world space
	vec3 color;
	mat4 lightMatrix;
};

struct PointLight {
	float kc; // constant term
	float kl; // linear term
	float kq; // quadratic term
	float far; // shadow matrix far plane
	float bias;
	vec3 pos; // in world space
	vec3 color;
};

layout(packed) uniform Lighting {
	DirLight dirLights[MAX_DIR_LIGHTS_COUNT];
	PointLight pointLights[MAX_POINT_LIGHTS_COUNT];

	uint pointLightsCount;
	uint dirLightsCount;

	float shadowOpacity;
	float diskRadius_k;
	float diskRadius_min;
};

uniform samplerCube pointLightShadowMaps[MAX_POINT_LIGHTS_COUNT];
uniform sampler2D dirLightShadowMaps[MAX_DIR_LIGHTS_COUNT];

struct LightingVars {
	vec3 viewDir;
	vec3 ambientResult; // result of ambient lighting for all lights
	vec3 diffuseResult; // result of diffuse lighting for all lights
	vec3 specularResult; // result of specular lighting for all lights

	// base lighting output variables
	vec3 lampEyePos; // Transformed lamp position into eye space
	vec3 ambient, diffuse, specular, lightDir;
} lighting;

void calculateBaseLighting(vec3 pos, vec3 color, float kc, float kl, float kq) {
	lighting.lampEyePos = vec3(lightingViewMatrix * vec4(pos, 1.0));
	lighting.lightDir = normalize(lighting.lampEyePos - viewPosition);

	float attenuation = attenuation(length(lighting.lampEyePos - viewPosition), kc, kl, kq);

	// ambient
	lighting.ambient = ambientStrength * color * attenuation;

	// diffuse
	float diff = diffuseLambert(viewNormal, lighting.lightDir);
	lighting.diffuse = diffuseStrength * diff * color * attenuation;

	// specular
	float spec = specularBlinnPhong(shininess, viewPosition, viewNormal, lighting.lightDir);
	lighting.specular = specularStrength * spec * color * attenuation;
}

void initLighting() {
	lighting.ambientResult = vec3(0.0);
	lighting.diffuseResult = vec3(0.0);
	lighting.specularResult = vec3(0.0);
	lighting.viewDir = normalize(-viewPosition);
}

void calculatePointLighting() {
	for (uint i = 0; i < pointLightsCount; i++) {
		calculateBaseLighting(pointLights[i].pos, pointLights[i].color, pointLights[i].kc, pointLights[i].kl, pointLights[i].kq);
		
		float shadow = pointLightSoftShadow(
			lightingWorldPos,
			pointLights[i].pos,
			length(viewPosition),
			diskRadius_k,
			diskRadius_min,
			pointLights[i].bias,
			pointLights[i].far,
			pointLightShadowMaps[i]
		) * shadowOpacity;

		lighting.ambientResult += lighting.ambient;
		lighting.diffuseResult += lighting.diffuse * (1 - shadow);
		lighting.specularResult += lighting.specular * (1 - shadow);
	}
}

void calculateDirLighting() {
	for (uint i = 0; i < dirLightsCount; i++) {
		calculateBaseLighting(dirLights[i].pos, dirLights[i].color, dirLights[i].kc, dirLights[i].kl, dirLights[i].kq);

		float shadow = dirLightSoftShadow(
			dirLights[i].lightMatrix,
			lightingWorldPos,
			viewNormal,
			lighting.lightDir,
			dirLights[i].minBias,
			dirLights[i].maxBias,
			dirLightShadowMaps[i]
		) * shadowOpacity;

		lighting.ambientResult += lighting.ambient;
		lighting.diffuseResult += lighting.diffuse * (1 - shadow);
		lighting.specularResult += lighting.specular * (1 - shadow);
	}
}

#endif // ALGINE_MODULE_LIGHTING_GLSL
