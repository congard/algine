in vec2 rawTexCoords;
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D color;
uniform sampler2D symbol;
uniform float opacity;

void main() {
    vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(symbol, rawTexCoords).r);
    fragColor = texture(color, texCoords) * sampled;
    fragColor.a *= opacity;
}
