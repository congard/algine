/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard

  COMPILER WILL REMOVE UNUSED VARIABLES
 */

#version 330 core

precision mediump float;					// Set the default precision to medium. We don't need as high of a
											// precision in the fragment shader.

#algdef

uniform vec3 viewPos; // Camera position
uniform float focalDepth; // if ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED
uniform float focalRange; // if ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED
uniform bool textureMappingEnabled;	// 0 false (color mapping), true (texture mapping) (if ALGINE_TEXTURE_MAPPING_MODE == ALGINE_TEXTURE_MAPPING_MODE_DUAL)

in mat4 model, view, vmMatrix;
in mat3 v_TBN, v_wTBN; // Tangent Bitangent Normal matrix and this matrix in world space
in vec4 v_Position; // Position for this fragment in world space
in vec3 v_Normal; // Interpolated normal for this fragment
in vec2 v_Texture; // Texture coordinates.
in float v_NormalMapping; // Is normal mapping enabled 0 - false, 1 - true (if ALGINE_NORMAL_MAPPING_MODE == ALGINE_NORMAL_MAPPING_MODE_DUAL)

uniform struct Mapping {
	#if defined ALGINE_NORMAL_MAPPING_MODE_ENABLED || defined ALGINE_NORMAL_MAPPING_MODE_DUAL
	sampler2D normal;
	#endif

	#if defined ALGINE_SSR_MODE_ENABLED
	sampler2D reflectionStrength;
	sampler2D jitter;
	#endif

	#ifdef ALGINE_LIGHTING_MODE_ENABLED

	#if defined ALGINE_TEXTURE_MAPPING_MODE_ENABLED || defined ALGINE_TEXTURE_MAPPING_MODE_DUAL
	sampler2D ambient;
	sampler2D diffuse;
	sampler2D specular;
	#endif

	#if defined ALGINE_TEXTURE_MAPPING_MODE_DISABLED || defined ALGINE_TEXTURE_MAPPING_MODE_DUAL
	vec4 cambient;
	vec4 cdiffuse;
	vec4 cspecular;
	#endif

	#else

	#if defined ALGINE_TEXTURE_MAPPING_MODE_ENABLED || defined ALGINE_TEXTURE_MAPPING_MODE_DUAL
	sampler2D diffuse;
	#endif

	#if defined ALGINE_TEXTURE_MAPPING_MODE_DISABLED || defined ALGINE_TEXTURE_MAPPING_MODE_DUAL
	vec4 cdiffuse;
	#endif

	#endif /* ALGINE_LIGHTING_MODE_ENABLED */
} mapping;

#ifdef ALGINE_LIGHTING_MODE_ENABLED
uniform int lampsCount;	// Lamps count

struct Lamp {
	float ambientStrength;
	float diffuseStrength;
	float specularStrength;
	float kc; // constant term
	float kl; // linear term
	float kq; // quadratic term
	int shininess;
	vec3 lampPos; // in world space
	vec3 lampColor;
};

uniform Lamp lamps[MAX_LAMPS_COUNT];
vec3 lampEyePos; // Transformed lamp position into eye space
#endif /* ALGINE_LIGHTING_MODE_ENABLED */

vec3 norm;
vec3 fragPos;
vec3 fragWorldPos;

// output colors
layout(location = 0) out vec4 fragColor;
layout(location = 1) out float dofBuffer;
layout(location = 2) out vec3 normalBuffer;
layout(location = 3) out vec2 ssrValuesBuffer;
layout(location = 4) out vec3 positionBuffer;

#if !defined ALGINE_SHADOW_MAPPING_MODE_DISABLED && defined ALGINE_LIGHTING_MODE_ENABLED
uniform samplerCube shadowMaps[MAX_LAMPS_COUNT];
uniform float far_plane_sm;	// shadow matrix far plane
uniform float shadow_bias;

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

float calculateShadow(vec3 lightDir, int index) {
	// get vector between fragment position and light position
	vec3 fragToLight = fragWorldPos - lamps[index].lampPos;
	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);
	// use the light to fragment vector to sample from the depth map
	float closestDepth;

	// PCF
	#ifdef ALGINE_SHADOW_MAPPING_MODE_ENABLED
	float viewDistance = length(viewPos - fragWorldPos);
	float diskRadius = (1.0 + (viewDistance / far_plane_sm)) * diskRadius_k + diskRadius_min;
	for (int i = 0; i < 20; i++) {
		closestDepth = texture(shadowMaps[index], fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane_sm; // Undo mapping [0;1]
		// now test for shadows
		if(currentDepth - shadow_bias > closestDepth) shadow += 1.0;
	}
	return shadow /= 20;
	#else
	closestDepth = texture(shadowMaps[index], fragToLight).r;
	closestDepth *= far_plane_sm; // Undo mapping [0;1]
	// now test for shadows
	return currentDepth - shadow_bias > closestDepth ? 1.0 : 0.0;
	#endif
}
#endif

#if defined ALGINE_LIGHTING_MODE_ENABLED && defined ALGINE_ATTENUATION_MODE_ENABLED
float calculateAttenuation(Lamp lamp) {
	float distance = length(lampEyePos - fragPos);
	return 1.0 / (
					lamp.kc +
					lamp.kl * distance +
					lamp.kq * (distance * distance)
			);
}
#endif


#define toVec4(v) vec4(v, 1.0)

// The entry point for our fragment shader.
void main() {
	fragWorldPos = vec3(model * v_Position);

	fragPos = vec3(vmMatrix * v_Position); // Transform the vertex into eye space

	#ifdef ALGINE_LIGHTING_MODE_ENABLED
	#ifdef ALGINE_NORMAL_MAPPING_MODE_DUAL
	if (v_NormalMapping == 0) norm = vec3(normalize(vmMatrix * vec4(v_Normal, 0)));
	else { // using normal map if normal mapping enabled
		norm = texture2D(mapping.normal, v_Texture).rgb;
		norm = normalize(norm * 2.0 - 1.0); // from [0; 1] to [-1; 1]
		norm = normalize(v_TBN * norm);
	}
	#elif defined ALGINE_NORMAL_MAPPING_MODE_ENABLED
	norm = texture2D(mapping.normal, v_Texture).rgb;
	norm = normalize(norm * 2.0 - 1.0); // from [0; 1] to [-1; 1]
	norm = normalize(v_TBN * norm);
	#else
	norm = vec3(normalize(vmMatrix * vec4(v_Normal, 0)));
	#endif /* ALGINE_NORMAL_MAPPING_MODE_DUAL */
	#endif

	#ifdef ALGINE_LIGHTING_MODE_ENABLED
	vec3 viewDir = normalize(mat3(view) * viewPos - fragPos);

	vec3 ambientResult = vec3(0, 0, 0); // result of ambient lighting for all lamps
	vec3 diffuseResult = vec3(0, 0, 0); // result of diffuse lighting for all lamps
	vec3 specularResult = vec3(0, 0, 0); // result of specular lighting for all lamps

	for (int i = 0; i < lampsCount; i++) {
		lampEyePos = vec3(view * toVec4(lamps[i].lampPos));

		#ifdef ALGINE_ATTENUATION_MODE_ENABLED
		// attenuation
		float attenuation = calculateAttenuation(lamps[i]);
		#else
		#define attenuation 1.0
		#endif /* ALGINE_ATTENUATION_MODE_ENABLED */

		// ambient
		vec3 ambient = lamps[i].ambientStrength * lamps[i].lampColor * attenuation;

		// diffuse
		vec3 lightDir = normalize(lampEyePos - fragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = lamps[i].diffuseStrength * diff * lamps[i].lampColor * attenuation;

		// specular
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), lamps[i].shininess);
		vec3 specular = lamps[i].specularStrength * spec * lamps[i].lampColor * attenuation;

		// result for this(i) lamp
		ambientResult += ambient;
		#if !defined ALGINE_SHADOW_MAPPING_MODE_DISABLED
		// calculate shadow
		shadow = calculateShadow(lightDir, i);
		diffuseResult += diffuse * (1 - shadow);
		specularResult += specular * (1 - shadow);
		#else
		diffuseResult += diffuse;
		specularResult += specular;
		#endif
	}

	#ifdef ALGINE_TEXTURE_MAPPING_MODE_ENABLED
	fragColor =
			toVec4(ambientResult) * texture(mapping.ambient, v_Texture) +
			toVec4(diffuseResult) * texture(mapping.diffuse, v_Texture) +
			toVec4(specularResult) * texture(mapping.specular, v_Texture);
	#elif defined ALGINE_TEXTURE_MAPPING_MODE_DISABLED
	fragColor =
			toVec4(ambientResult) * mapping.cambient +
			toVec4(diffuseResult) * mapping.cdiffuse +
			toVec4(specularResult) * mapping.cspecular;
	#else
	if (textureMappingEnabled)
		fragColor =
				toVec4(ambientResult) * texture(mapping.ambient, v_Texture) +
				toVec4(diffuseResult) * texture(mapping.diffuse, v_Texture) +
				toVec4(specularResult) * texture(mapping.specular, v_Texture);
	else
		fragColor =
				toVec4(ambientResult) * mapping.cambient +
				toVec4(diffuseResult) * mapping.cdiffuse +
				toVec4(specularResult) * mapping.cspecular;
	#endif /* ALGINE_TEXTURE_MAPPING_MODE_ENABLED */

	#else
		#ifdef ALGINE_TEXTURE_MAPPING_MODE_ENABLED
		fragColor = texture(mapping.diffuse, v_Texture);
		#elif defined ALGINE_TEXTURE_MAPPING_MODE_DISABLED
		fragColor = mapping.cdiffuse;
		#else
		if (textureMappingEnabled) fragColor = texture(mapping.diffuse, v_Texture);
		else fragColor = mapping.cdiffuse;
		#endif /* ALGINE_TEXTURE_MAPPING_MODE_ENABLED */
	#endif /* ALGINE_LIGHTING_MODE_ENABLED */

	#ifdef ALGINE_DOF_MODE_ENABLED
	dofBuffer = abs(focalDepth + fragPos.z) / focalRange;
	#endif

	#ifdef ALGINE_SSR_MODE_ENABLED
	normalBuffer = norm;
	positionBuffer = fragPos;
	ssrValuesBuffer.r = texture(mapping.reflectionStrength, v_Texture).r;
	ssrValuesBuffer.g = texture(mapping.jitter, v_Texture).r;
	#endif
}