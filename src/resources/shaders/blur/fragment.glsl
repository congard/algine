// Algine blur fragment shader

#version 330

#algdef

in vec2 texCoord;

uniform sampler2D image;
uniform float kernel[KERNEL_RADIUS];

layout (location = 0) out vecout fragColor;

void main() {
	vec2 texOffset = 1.0 / textureSize(image, 0); // gets size of single texel
    fragColor = texture(image, texCoord).texComponent * kernel[0]; // current fragment’s contribution

	#ifdef ALGINE_HORIZONTAL
		#define _texCoordP texCoord + vec2(texOffset.x * i, 0.0)
		#define _texCoordM texCoord - vec2(texOffset.x * i, 0.0)
	#else
		#define _texCoordP texCoord + vec2(0.0, texOffset.y * i)
		#define _texCoordM texCoord - vec2(0.0, texOffset.y * i)
	#endif

    for(int i = 1; i < KERNEL_RADIUS; i++) {
		fragColor += kernel[i] * (texture(image, _texCoordP).texComponent + texture(image, _texCoordM).texComponent);
	}
}