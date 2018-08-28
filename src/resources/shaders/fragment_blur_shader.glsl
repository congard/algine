/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330

#define KERNEL_SIZE 8

out vec4 fragColor;
in vec2 texCoords;

uniform sampler2D image;
uniform bool isHorizontal;
uniform float kernel[KERNEL_SIZE];

void main() {
	vec2 texOffset = 1.0 / textureSize(image, 0); // gets size of single texel
	vec3 result = texture(image, texCoords).rgb * kernel[0]; // current fragment’s contribution
	if(isHorizontal) {
		for(int i = 1; i < kernel.length(); i++) {
			result += texture(image, texCoords + vec2(texOffset.x * i, 0.0)).rgb * kernel[i];
			result += texture(image, texCoords - vec2(texOffset.x * i, 0.0)).rgb * kernel[i];
		}
	} else {
		for(int i = 1; i < kernel.length(); ++i) {
			result += texture(image, texCoords + vec2(0.0, texOffset.y * i)).rgb * kernel[i];
			result += texture(image, texCoords - vec2(0.0, texOffset.y * i)).rgb * kernel[i];
		}
	}
	fragColor = vec4(result, 1.0);
}
