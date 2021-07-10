in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D color;
uniform sampler2D char;

void main() {
    vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(char, texCoords).r);
    fragColor = texture(color, texCoords) * sampled;
}
