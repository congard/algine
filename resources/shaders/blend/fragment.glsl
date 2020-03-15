#version 330

#pragma algine include "modules/BlendBloom.glsl"

out vec3 fragColor;
in vec2 texCoord;

uniform sampler2D bloom;
uniform sampler2D image;
uniform float exposure;
uniform float gamma;

void main() {
	vec3 color;
	if (isBloomEnabled())
		color = blendBloom(texture(bloom, texCoord).rgb, texture(image, texCoord).rgb);
	else
		color = texture(image, texCoord).rgb;
	
	fragColor = vec3(1.0) - exp(-color * exposure); // tone mapping
	fragColor = pow(fragColor, vec3(1.0 / gamma)); // gamma correction
}
