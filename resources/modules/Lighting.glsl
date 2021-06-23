/**
 * Lighting module
 * Include it in fragment shader
 * Necessary variables:

    mat4 lightingViewMatrix;
    vec3 lightingCameraPos;
    vec3 lightingWorldPos; // position for this fragment in world space, need for shadow mapping

 * You must link these variables with yours
 */

#ifndef ALGINE_MODULE_LIGHTING_GLSL
#define ALGINE_MODULE_LIGHTING_GLSL

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

#if !defined ALGINE_SHADOW_MAPPING_MODE_DISABLED
// for PCF
const vec3 sampleOffsetDirections[20] = vec3[] (
		vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
		vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
		vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
		vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
		vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

float calculatePointLightShadow(uint index) {
	// get vector between fragment position and light position
	vec3 fragToLight = lightingWorldPos - pointLights[index].pos;
	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);
	// use the light to fragment vector to sample from the depth map
	float closestDepth;
	float shadow = 0;

	// PCF
	#ifdef ALGINE_SHADOW_MAPPING_MODE_ENABLED
	float viewDistance = length(lightingCameraPos - lightingWorldPos);
	float diskRadius = (1.0 + (viewDistance / pointLights[index].far)) * diskRadius_k + diskRadius_min;
	for (int i = 0; i < 20; i++) {
		closestDepth = texture(pointLightShadowMaps[index], fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= pointLights[index].far; // Undo mapping [0;1]
		// now test for shadows
		if (currentDepth - pointLights[index].bias > closestDepth)
			shadow += 1.0;
	}
	return shadow /= 20;
	#else
	closestDepth = texture(pointLightShadowMaps[index], fragToLight).r;
	closestDepth *= pointLights[index].far; // Undo mapping [0;1]
	// now test for shadows
	return currentDepth - pointLights[index].bias > closestDepth ? 1.0 : 0.0;
	#endif
}

float calculateDirLightShadow(uint index) {
	vec4 fragPosLightSpace = dirLights[index].lightMatrix * vec4(lightingWorldPos, 1.0);
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light’s perspective (using [0; 1] range projCoords as coords)
	float closestDepth = texture(dirLightShadowMaps[index], projCoords.xy).r;
	// get depth of current fragment from light’s perspective
	float currentDepth = projCoords.z;
	
	/*
	 * Here we have a maximum bias of 0.05 and a minimum of 0.005 based on the surface’s normal and
	 * light direction. This way surfaces like the floor that are almost perpendicular to the light source get a small
	 * bias, while surfaces like the cube’s side-faces get a much larger bias.
	*/
	float bias = max(dirLights[index].maxBias * (1.0 - dot(viewNormal, lighting.lightDir)), dirLights[index].minBias);

	// soft shadow pcf 3*3
	#ifdef ALGINE_SHADOW_MAPPING_MODE_ENABLED // PCF
	vec2 texelSize = 1.0 / textureSize(dirLightShadowMaps[index], 0);
	float shadow = 0;
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			float pcfDepth = texture(dirLightShadowMaps[index], projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	return shadow /= 9.0;

	#else
	return currentDepth - bias > closestDepth ? 1.0 : 0.0; // simple shadow
	#endif
}
#endif

float calculateAttenuation(float kc, float kl, float kq) {
	float distance = length(lighting.lampEyePos - viewPosition);

	return 1.0 / (kc + kl * distance + kq * (distance * distance));
}

void calculateBaseLighting(vec3 pos, vec3 color, float kc, float kl, float kq) {
	lighting.lampEyePos = vec3(lightingViewMatrix * vec4(pos, 1.0));

	float attenuation = 1.0;

	#ifdef ALGINE_ATTENUATION_MODE_ENABLED
	attenuation = calculateAttenuation(kc, kl, kq);
	#endif

	// ambient
	lighting.ambient = ambientStrength * color * attenuation;

	// diffuse
	lighting.lightDir = normalize(lighting.lampEyePos - viewPosition);
	float diff = max(dot(viewNormal, lighting.lightDir), 0.0);
	lighting.diffuse = diffuseStrength * diff * color * attenuation;

	// specular
	vec3 reflectDir = reflect(-lighting.lightDir, viewNormal);
	float spec = pow(max(dot(lighting.viewDir, reflectDir), 0.0), shininess);
	lighting.specular = specularStrength * spec * color * attenuation;
}

void initLighting() {
	lighting.ambientResult = vec3(0.0);
	lighting.diffuseResult = vec3(0.0);
	lighting.specularResult = vec3(0.0);
	lighting.viewDir = normalize(mat3(lightingViewMatrix) * lightingCameraPos - viewPosition);
}

void calculateDirLighting() {
	for (uint i = 0; i < pointLightsCount; i++) {
		calculateBaseLighting(pointLights[i].pos, pointLights[i].color, pointLights[i].kc, pointLights[i].kl, pointLights[i].kq);
		
		float shadow = 0;

		#if !defined ALGINE_SHADOW_MAPPING_MODE_DISABLED
		shadow = calculatePointLightShadow(i) * shadowOpacity;
		#endif
		
		lighting.ambientResult += lighting.ambient;
		lighting.diffuseResult += lighting.diffuse * (1 - shadow);
		lighting.specularResult += lighting.specular * (1 - shadow);
	}
}

void calculatePointLighting() {
	for (uint i = 0; i < dirLightsCount; i++) {
		calculateBaseLighting(dirLights[i].pos, dirLights[i].color, dirLights[i].kc, dirLights[i].kl, dirLights[i].kq);

		float shadow = 0;

		#if !defined ALGINE_SHADOW_MAPPING_MODE_DISABLED
		shadow = calculateDirLightShadow(i) * shadowOpacity;
		#endif

		lighting.ambientResult += lighting.ambient;
		lighting.diffuseResult += lighting.diffuse * (1 - shadow);
		lighting.specularResult += lighting.specular * (1 - shadow);
	}
}

#endif // ALGINE_MODULE_LIGHTING_GLSL
