in vec2 texCoord;

uniform sampler2D image;
uniform float kernel[KERNEL_RADIUS];

layout (location = 0) out vecout fragColor;

void main() {
    vec2 texelSize = 1.0f / vec2(textureSize(image, 0)); // gets size of single texel

    fragColor = texture(image, texCoord).texComponent * kernel[0]; // current fragment’s contribution

    #ifdef ALGINE_HORIZONTAL
        #define texCoordP texCoord + vec2(texelSize.x * iteration, 0.0f)
        #define texCoordM texCoord - vec2(texelSize.x * iteration, 0.0f)
    #else
        #define texCoordP texCoord + vec2(0.0f, texelSize.y * iteration)
        #define texCoordM texCoord - vec2(0.0f, texelSize.y * iteration)
    #endif

    for (int i = 1; i < KERNEL_RADIUS; i++) {
        const float iteration = float(i);
        fragColor += kernel[i] * (texture(image, texCoordP).texComponent + texture(image, texCoordM).texComponent);
    }
}
