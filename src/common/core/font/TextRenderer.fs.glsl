in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D char;
uniform vec4 charColor;

void main() {
    vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(char, texCoords).r);
    fragColor = charColor * sampled;
}