in vec2 pos;
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D color;
uniform vec2 origin;
uniform float radius;
uniform bool antialiasing;

void main() {
    float d = length(pos - origin);

    if (d <= radius) {
        fragColor = texture(color, texCoords);

        if (antialiasing) {
            float delta = 1.5f;
            float alpha = smoothstep(radius - delta, radius, d);

            fragColor.a *= 1 - alpha;
        }
    } else {
        discard;
    }
}
