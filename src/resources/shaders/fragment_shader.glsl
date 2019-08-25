/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard

  COMPILER WILL REMOVE UNUSED VARIABLES
 */

// some GPUs can work with 330 (basically Nvidia)
#version 400 core

$ALGINE_DEFINITIONS

uniform mat4 viewMatrix;
uniform vec3 cameraPos;
uniform bool textureMappingEnabled;	// ALGINE_TEXTURE_MAPPING_MODE_DUAL
uniform bool u_NormalMapping; // ALGINE_NORMAL_MAPPING_MODE_DUAL

uniform float shadowOpacity = 1.0;

uniform uint pointLightsCount; // Point lights count
uniform uint dirLightsCount; // Dir lights count

in mat3 v_TBN; // Tangent Bitangent Normal matrix
in vec3 worldPosition; // Position for this fragment in world space
in vec3 viewNormal; // Interpolated normal for this fragment, needs if normal mapping disabled
in vec3 viewPosition;
in vec2 texCoord; // Texture coordinates.

uniform struct Material {
	sampler2D normal; // normal mapping sampler
	
	sampler2D reflectionStrength; // ssr samplers
	sampler2D jitter;
	
	sampler2D ambient;
	sampler2D diffuse;
	sampler2D specular;
	
	vec4 cambient;
	vec4 cdiffuse;
	vec4 cspecular;

	float ambientStrength;
	float diffuseStrength;
	float specularStrength;
	float shininess;
} material;

uniform struct PointLight {
	float kc; // constant term
	float kl; // linear term
	float kq; // quadratic term
	float far; // shadow matrix far plane
	float bias;
	vec3 pos; // in world space
	vec3 color;
}, pointLights[MAX_POINT_LIGHTS_COUNT];
uniform samplerCube pointLightShadowMaps[MAX_POINT_LIGHTS_COUNT];

uniform struct DirLight {
	float kc; // constant term
	float kl; // linear term
	float kq; // quadratic term
	float minBias, maxBias;
	vec3 pos; // in world space
	vec3 color;
	mat4 lightMatrix;
}, dirLights[MAX_DIR_LIGHTS_COUNT];
uniform sampler2D dirLightShadowMaps[MAX_DIR_LIGHTS_COUNT];

vec3 lampEyePos; // Transformed lamp position into eye space
vec3 norm;
vec3 ambient, diffuse, specular, viewDir, lightDir; // base lighting variables

// output colors
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 normalBuffer;
layout(location = 2) out vec3 positionBuffer;
layout(location = 3) out vec2 ssrValuesBuffer;

#if !defined ALGINE_SHADOW_MAPPING_MODE_DISABLED && defined ALGINE_LIGHTING_MODE_ENABLED
float shadow;

#ifdef ALGINE_SHADOW_MAPPING_MODE_ENABLED
uniform float diskRadius_k;
uniform float diskRadius_min;

// for PCF
const vec3 sampleOffsetDirections[20] = vec3[] (
		vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
		vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
		vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
		vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
		vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);
#endif /* ALGINE_SHADOW_MAPPING_MODE_ENABLED */

float calculatePointLightShadow(uint index) {
	// get vector between fragment position and light position
	vec3 fragToLight = worldPosition - pointLights[index].pos;
	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);
	// use the light to fragment vector to sample from the depth map
	float closestDepth;

	// PCF
	#ifdef ALGINE_SHADOW_MAPPING_MODE_ENABLED
	float viewDistance = length(cameraPos - worldPosition);
	float diskRadius = (1.0 + (viewDistance / pointLights[index].far)) * diskRadius_k + diskRadius_min;
	for (int i = 0; i < 20; i++) {
		closestDepth = texture(pointLightShadowMaps[index], fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= pointLights[index].far; // Undo mapping [0;1]
		// now test for shadows
		if(currentDepth - pointLights[index].bias > closestDepth) shadow += 1.0;
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
	vec4 fragPosLightSpace = dirLights[index].lightMatrix * vec4(worldPosition, 1.0);
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
	float bias = max(dirLights[index].maxBias * (1.0 - dot(norm, lightDir)), dirLights[index].minBias);

	// soft shadow pcf 3*3
	#ifdef ALGINE_SHADOW_MAPPING_MODE_ENABLED // PCF
	vec2 texelSize = 1.0 / textureSize(dirLightShadowMaps[index], 0);
	shadow = 0;
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
	float distance = length(lampEyePos - viewPosition);
	return 1.0 / (
					kc +
					kl * distance +
					kq * (distance * distance)
			);
}

#define toVec4(v) vec4(v, 1.0)

// kclq: vec3(kc, kl, kq)
void calculateBaseLighting(vec3 pos, vec3 color, float kc, float kl, float kq) {
	lampEyePos = vec3(viewMatrix * toVec4(pos));

	#ifdef ALGINE_ATTENUATION_MODE_ENABLED
	// attenuation
	float attenuation = calculateAttenuation(kc, kl, kq);
	#else
	#define attenuation 1.0
	#endif /* ALGINE_ATTENUATION_MODE_ENABLED */

	// ambient
	ambient = material.ambientStrength * color * attenuation;

	// diffuse
	lightDir = normalize(lampEyePos - viewPosition);
	float diff = max(dot(norm, lightDir), 0.0);
	diffuse = material.diffuseStrength * diff * color * attenuation;

	// specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	specular = material.specularStrength * spec * color * attenuation;
}

// The entry point for our fragment shader.
void main() {
	#ifdef ALGINE_LIGHTING_MODE_ENABLED
	#ifdef ALGINE_NORMAL_MAPPING_MODE_DUAL
	if (u_NormalMapping == 0) norm = viewNormal;
	else { // using normal map if normal mapping enabled
		norm = texture2D(material.normal, texCoord).rgb;
		norm = normalize(norm * 2.0 - 1.0); // from [0; 1] to [-1; 1]
		norm = normalize(v_TBN * norm);
	}
	#elif defined ALGINE_NORMAL_MAPPING_MODE_ENABLED
	norm = texture2D(material.normal, texCoord).rgb;
	norm = normalize(norm * 2.0 - 1.0); // from [0; 1] to [-1; 1]
	norm = normalize(v_TBN * norm);
	#else
	norm = viewNormal;
	#endif /* ALGINE_NORMAL_MAPPING_MODE_DUAL */
	#endif

	#ifdef ALGINE_LIGHTING_MODE_ENABLED
	viewDir = normalize(mat3(viewMatrix) * cameraPos - viewPosition);

	vec3 ambientResult = vec3(0, 0, 0); // result of ambient lighting for all lights
	vec3 diffuseResult = vec3(0, 0, 0); // result of diffuse lighting for all lights
	vec3 specularResult = vec3(0, 0, 0); // result of specular lighting for all lights

	for (uint i = 0; i < pointLightsCount; i++) {
		calculateBaseLighting(pointLights[i].pos, pointLights[i].color, pointLights[i].kc, pointLights[i].kl, pointLights[i].kq);
		
		#if !defined ALGINE_SHADOW_MAPPING_MODE_DISABLED
		// calculate shadow
		shadow = calculatePointLightShadow(i) * shadowOpacity;
		#else
		shadow = 0;
		#endif
		
		ambientResult += ambient;
		diffuseResult += diffuse * (1 - shadow);
		specularResult += specular * (1 - shadow);
	}

	for (uint i = 0; i < dirLightsCount; i++) {
		calculateBaseLighting(dirLights[i].pos, dirLights[i].color, dirLights[i].kc, dirLights[i].kl, dirLights[i].kq);

		#if !defined ALGINE_SHADOW_MAPPING_MODE_DISABLED
		// calculate shadow
		shadow = calculateDirLightShadow(i) * shadowOpacity;
		#else
		shadow = 0;
		#endif

		ambientResult += ambient;
		diffuseResult += diffuse * (1 - shadow);
		specularResult += specular * (1 - shadow);
	}

	#ifdef ALGINE_TEXTURE_MAPPING_MODE_ENABLED
	fragColor =
			toVec4(ambientResult) * texture(material.ambient, texCoord) +
			toVec4(diffuseResult) * texture(material.diffuse, texCoord) +
			toVec4(specularResult) * texture(material.specular, texCoord);
	#elif defined ALGINE_TEXTURE_MAPPING_MODE_DISABLED
	fragColor =
			toVec4(ambientResult) * material.cambient +
			toVec4(diffuseResult) * material.cdiffuse +
			toVec4(specularResult) * material.cspecular;
	#else
	if (textureMappingEnabled)
		fragColor =
				toVec4(ambientResult) * texture(material.ambient, texCoord) +
				toVec4(diffuseResult) * texture(material.diffuse, texCoord) +
				toVec4(specularResult) * texture(material.specular, texCoord);
	else
		fragColor =
				toVec4(ambientResult) * material.cambient +
				toVec4(diffuseResult) * material.cdiffuse +
				toVec4(specularResult) * material.cspecular;
	#endif /* ALGINE_TEXTURE_MAPPING_MODE_ENABLED */

	#else /* ALGINE_LIGHTING_MODE_DISABLED */
		#ifdef ALGINE_TEXTURE_MAPPING_MODE_ENABLED
		fragColor = texture(material.diffuse, texCoord);
		#elif defined ALGINE_TEXTURE_MAPPING_MODE_DISABLED
		fragColor = material.cdiffuse;
		#else
		if (textureMappingEnabled) fragColor = texture(material.diffuse, texCoord);
		else fragColor = material.cdiffuse;
		#endif /* ALGINE_TEXTURE_MAPPING_MODE_ENABLED */
	#endif /* ALGINE_LIGHTING_MODE_XXX */

	#ifdef ALGINE_SSR_MODE_ENABLED
	normalBuffer = norm;
	positionBuffer = viewPosition;
	ssrValuesBuffer.r = texture(material.reflectionStrength, texCoord).r;
	ssrValuesBuffer.g = texture(material.jitter, texCoord).r;
	#endif
}
