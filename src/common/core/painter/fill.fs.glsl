in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D color;

void main() {
    fragColor = texture(color, texCoords);
}
