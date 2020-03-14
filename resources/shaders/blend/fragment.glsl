// Algine blend fragment shader

#version 330

out vec3 fragColor;
in vec2 texCoord;

uniform sampler2D bloom;
uniform sampler2D image;
uniform float exposure;
uniform float gamma;

void main() {
	#ifdef ALGINE_BLOOM_TYPE_ADD // additive blending
		vec3 color = texture(bloom, texCoord).rgb + texture(image, texCoord).rgb;
	#elif defined ALGINE_BLOOM_TYPE_SCREEN // screen blending
		vec3 color = 1 - (1 - texture(bloom, texCoord).rgb) * (1 - texture(image, texCoord).rgb);
	#else
		vec3 color = texture(image, texCoord).rgb;
	#endif

	fragColor = vec3(1.0) - exp(-color * exposure); // tone mapping
	fragColor = pow(fragColor, vec3(1.0 / gamma)); // gamma correction
}
