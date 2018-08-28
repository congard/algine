/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330 core

precision mediump float;					// Set the default precision to medium. We don't need as high of a
											// precision in the fragment shader.

#define MAX_LAMPS_COUNT 8					// Max lamps count

uniform vec3 u_ViewPos;						// Camera position
uniform int u_LampsCount;					// Lamps count
uniform float brightnessThreshold = 0.3;	// brightness threshold variable
uniform float far_plane;	 				// shadow matrix far plane

in mat4 model, view, projection;
in mat3 v_TBN;							// Tangent Bitangent Normal matrix
in vec4 v_Position;						// Position for this fragment in world space
in vec3 v_Normal;						// Interpolated normal for this fragment.
in vec2 v_Texture;						// Texture coordinates.
in float v_NormalMapping;				// Is normal mapping enabled 0 - false, 1 - true

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

uniform samplerCube shadowMaps[MAX_LAMPS_COUNT];

uniform struct Mapping {
	sampler2D ambient;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
} u_Mapping;

uniform Lamp u_Lamps[MAX_LAMPS_COUNT];

vec3 norm;
vec3 fragPos;
vec3 fragWorldPos;
vec3 lampEyePos; // Transformed lamp position into eye space
float shadow;

// for PCF
vec3 sampleOffsetDirections[20] = vec3[] (
		vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
		vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
		vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
		vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
		vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

// output colors
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 fragBrightColor;

float calculateShadow(vec3 lightDir, int index) {
	// get vector between fragment position and light position
	vec3 fragToLight = fragWorldPos - u_Lamps[index].lampPos;
	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);
	// bias
	float bias = 0.6;
	// use the light to fragment vector to sample from the depth map
	float closestDepth;

	// PCF
	float viewDistance = length(u_ViewPos - fragWorldPos);
	float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
	for (int i = 0; i < 20; ++i) {
		closestDepth = texture(shadowMaps[index], fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane; // Undo mapping [0;1]
		// now test for shadows
		if(currentDepth - bias > closestDepth) shadow += 1.0;
	}
	shadow /= 20;

	//fragColor = vec4(vec3(closestDepth / far_plane), 1.0); // visualizing
	return shadow;
}

float calculateAttenuation(Lamp lamp) {
	float distance = length(lampEyePos - fragPos);
	return 1.0 / (
					lamp.kc +
					lamp.kl * distance +
					lamp.kq * (distance * distance)
			);
}

vec4 toVec4(vec3 v) {
	return vec4(v, 1);
}

// The entry point for our fragment shader.
void main() {
	fragWorldPos = vec3(model * v_Position);
	 // Transform the vertex into eye space
	mat4 mvMatrix = view * model;
	fragPos = vec3(mvMatrix * v_Position);

	vec3 viewDir = normalize(mat3(view) * u_ViewPos - fragPos);
	if (v_NormalMapping == 0) norm = vec3(normalize(mvMatrix * vec4(v_Normal, 0)));
	else { // using normal map if normal mapping enabled
		norm = texture2D(u_Mapping.normal, v_Texture).rgb;
		norm = normalize(norm * 2.0 - 1.0); // from [0; 1] to [-1; 1]
		norm = normalize(v_TBN * norm);
	}

	vec3 ambientResult = vec3(0, 0, 0); // result of ambient lighting for all lamps
	vec3 diffuseResult = vec3(0, 0, 0); // result of diffuse lighting for all lamps
	vec3 specularResult = vec3(0, 0, 0); // result of specular lighting for all lamps

	for (int i = 0; i < u_LampsCount; i++) {
		lampEyePos = vec3(view * toVec4(u_Lamps[i].lampPos));
		// attenuation
		float attenuation = calculateAttenuation(u_Lamps[i]);

		// ambient
		vec3 ambient = u_Lamps[i].ambientStrength * u_Lamps[i].lampColor * attenuation;

		// diffuse
		vec3 lightDir = normalize(lampEyePos - fragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = u_Lamps[i].diffuseStrength * diff * u_Lamps[i].lampColor * attenuation;

		// specular
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Lamps[i].shininess);
		vec3 specular = u_Lamps[i].specularStrength * spec * u_Lamps[i].lampColor * attenuation;

		// calculate shadow
		shadow = calculateShadow(lightDir, i);

		// result for this(i) lamp
		ambientResult += ambient;
		diffuseResult += diffuse * (1 - shadow);
		specularResult += specular * (1 - shadow);
	}
	
	fragColor =
			toVec4(ambientResult) * texture(u_Mapping.ambient, v_Texture) +
			toVec4(diffuseResult) * texture(u_Mapping.diffuse, v_Texture) +
			toVec4(specularResult) * texture(u_Mapping.specular, v_Texture);

	// brightness calculation
	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > brightnessThreshold) fragBrightColor = vec4(fragColor.rgb, 1.0);
}
