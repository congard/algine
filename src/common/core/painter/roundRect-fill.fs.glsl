in vec2 pos;
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D color;
uniform vec4 p1p2; // p1(x, y); p2(z, w)
uniform vec4 radius;
uniform vec4 scale; // y-scale
uniform bool antialiasing;

#define r1 radius.x
#define r2 radius.y
#define r3 radius.z
#define r4 radius.w

#define s1 scale.x
#define s2 scale.y
#define s3 scale.z
#define s4 scale.w

#define antialiasingDelta 1.5f

// If you know how, please,
// optimize this shader
// A --------- B
//  |         |
//  |         |
//  |         |
// C --------- D
// A(x, y); r1; s1
// B(z, y); r2; s2
// C(x, w); r3; s3
// D(z, w); r4; s4

// nearest radius & distance for corner
float nearest_r = -2.0f;
float nearest_d = -1.0f;

bool inCorner(vec2 p, float sy, float r) {
    return pos.x >= p.x && pos.x <= p.x + r && pos.y >= p.y && pos.y <= p.y + r * sy;
}

bool outsideEllipse(vec2 origin, float sy, float r) {
    vec2 d = pos - origin;
    d.y /= sy;

    float dist = length(d);

    if (nearest_d > dist || nearest_d == -1.0f) {
        nearest_d = dist;
        nearest_r = r;
    }

    return dist > r;
}

bool isDiscard(vec2 cornerStart, vec2 ellipseOrigin, float sy, float r) {
    return inCorner(cornerStart, sy, r) && outsideEllipse(ellipseOrigin, sy, r);
}

float getAlpha(float d0, float d) {
    float alpha = smoothstep(0.0f, antialiasingDelta, d);
    return alpha;
}

void main() {
    if (isDiscard(p1p2.xy,                  p1p2.xy + vec2(r1, r1 * s1), s1, r1) ||
        isDiscard(p1p2.zy - vec2(r2, 0.0f), p1p2.zy + vec2(-r2, r2 * s2), s2, r2) ||
        isDiscard(p1p2.xw - vec2(0.0f, r3), p1p2.xw + vec2(r3, -r3 * s3), s3, r3) ||
        isDiscard(p1p2.zw - r4,             p1p2.zw - vec2(r4, r4 * s4), s4, r4))
    {
        discard;
    } else {
        fragColor = texture(color, texCoords);

        if (!antialiasing) {
            return;
        }

        float antialiasedAlpha = 1.0f;

        if (nearest_d <= nearest_r) {
            float alpha = smoothstep(nearest_r - antialiasingDelta, nearest_r, nearest_d);

            antialiasedAlpha = 1.0f - alpha;
        }

        float w = p1p2.z - p1p2.x;
        float h = p1p2.w - p1p2.y;
        float alpha = getAlpha(w / 2.0f, pos.x - p1p2.x);
        alpha *= getAlpha(w / 2.0f, p1p2.z - pos.x);
        alpha *= getAlpha(h / 2.0f, pos.y - p1p2.y);
        alpha *= getAlpha(h / 2.0f, p1p2.w - pos.y);

        fragColor.a *= min(antialiasedAlpha, alpha);
    }
}
