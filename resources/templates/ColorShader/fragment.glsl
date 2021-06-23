// some GPUs can work with 330 (basically Nvidia)
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
uniform vec3 cameraPos;
uniform bool textureMappingEnabled;	// ALGINE_TEXTURE_MAPPING_MODE_DUAL

in mat3 matTBN;
in vec3 worldPosition; // Position for this fragment in world space
in vec3 viewPosition;
in vec2 texCoord; // Texture coordinates.

// output colors
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 normalBuffer;
layout(location = 2) out vec3 positionBuffer;
layout(location = 3) out vec2 ssrValuesBuffer;

vec3 viewNormal;

#alp include <NormalMapping.fs>

#alp link viewMatrix lightingViewMatrix
#alp link cameraPos lightingCameraPos
#alp link worldPosition lightingWorldPos
#alp include "modules/Lighting.glsl"

// The entry point for our fragment shader.
void main() {
	viewNormal = getNormal(normal, texCoord, matTBN);

	initLighting();
	calculateDirLighting();
	calculatePointLighting();

	fragColor =
			vec4(lighting.ambientResult, 1.0f) * texture(ambient, texCoord) +
			vec4(lighting.diffuseResult, 1.0f) * texture(diffuse, texCoord) +
			vec4(lighting.specularResult, 1.0f) * texture(specular, texCoord);

	normalBuffer = viewNormal;
	positionBuffer = viewPosition;
	ssrValuesBuffer.r = texture(reflectionStrength, texCoord).r;
	ssrValuesBuffer.g = texture(jitter, texCoord).r;
}
