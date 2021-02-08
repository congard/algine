uniform vec3 threshold = vec3(0.2126, 0.7152, 0.0722);
uniform float brightnessThreshold = 0.3;
uniform sampler2D image;

in vec2 texCoord;

layout (location = 0) out vec3 fragColor;

void main() {
    vec3 color = texture(image, texCoord).rgb;
    if (dot(color, threshold) > brightnessThreshold) fragColor = color;
}