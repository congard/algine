/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * github.com/congard
 */

// some GPUs can work with 330 (basically Nvidia)
#version 400 core

uniform mat4 viewMatrix;
uniform vec3 cameraPos;
uniform bool textureMappingEnabled;	// ALGINE_TEXTURE_MAPPING_MODE_DUAL

in vec3 worldPosition; // Position for this fragment in world space
in vec3 viewNormal; // Interpolated normal for this fragment, needs if normal mapping disabled
in vec3 viewPosition;
in vec2 texCoord; // Texture coordinates.

// output colors
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec3 normalBuffer;
layout(location = 2) out vec3 positionBuffer;
layout(location = 3) out vec2 ssrValuesBuffer;

#alp include "modules/Material.glsl"

#alp link material.normal normalMappingNormalMap
#alp include "modules/NormalMapping.glsl"

#alp link viewMatrix lightingViewMatrix
#alp link cameraPos lightingCameraPos
#alp link worldPosition lightingWorldPos
#alp include "modules/Lighting.glsl"

// The entry point for our fragment shader.
void main() {
	initNormalMapping();

	#ifdef ALGINE_LIGHTING_MODE_ENABLED
	initLighting();
	calculateDirLighting();
	calculatePointLighting();

	#define toVec4(v) vec4(v, 1.0)
	#ifdef ALGINE_TEXTURE_MAPPING_MODE_ENABLED
	fragColor =
			toVec4(lighting.ambientResult) * texture(material.ambient, texCoord) +
			toVec4(lighting.diffuseResult) * texture(material.diffuse, texCoord) +
			toVec4(lighting.specularResult) * texture(material.specular, texCoord);
	#elif defined ALGINE_TEXTURE_MAPPING_MODE_DISABLED
	fragColor =
			toVec4(lighting.ambientResult) * material.cambient +
			toVec4(lighting.diffuseResult) * material.cdiffuse +
			toVec4(lighting.specularResult) * material.cspecular;
	#else
	if (textureMappingEnabled)
		fragColor =
				toVec4(lighting.ambientResult) * texture(material.ambient, texCoord) +
				toVec4(lighting.diffuseResult) * texture(material.diffuse, texCoord) +
				toVec4(lighting.specularResult) * texture(material.specular, texCoord);
	else
		fragColor =
				toVec4(lighting.ambientResult) * material.cambient +
				toVec4(lighting.diffuseResult) * material.cdiffuse +
				toVec4(lighting.specularResult) * material.cspecular;
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
	normalBuffer = normalMapping.viewNormal;
	positionBuffer = viewPosition;
	ssrValuesBuffer.r = texture(material.reflectionStrength, texCoord).r;
	ssrValuesBuffer.g = texture(material.jitter, texCoord).r;
	#endif
}
