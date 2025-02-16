in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D color;
uniform float opacity;

void main() {
    fragColor = texture(color, texCoords);
    fragColor.a *= opacity;
}
