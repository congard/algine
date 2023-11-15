#include "hsv.h"

#include <cmath>

namespace algine::internal {
void rgb2hsv(float r, float g, float b, float &h, float &s, float &v) {
    float cMax = std::max(std::max(r, g), b);
    float cMin = std::min(std::min(r, g), b);
    float delta = cMax - cMin;

    if (delta > 0) {
        if (cMax == r) {
            h = 60 * std::fmod(((g - b) / delta), 6.0f);
        } else if (cMax == g) {
            h = 60 * (((b - r) / delta) + 2);
        } else if (cMax == b) {
            h = 60 * (((r - g) / delta) + 4);
        }

        if (cMax > 0) {
            s = delta / cMax;
        } else {
            s = 0;
        }

        v = cMax;
    } else {
        h = 0;
        s = 0;
        v = cMax;
    }

    if (h < 0) {
        h = 360 + h;
    }
}

void hsv2rgb(float &r, float &g, float &b, float h, float s, float v) {
    float c = v * s; // Chroma
    float hPrime = std::fmod(h / 60.0f, 6.0f);
    float x = c * (1 - std::fabs(std::fmod(hPrime, 2.0f) - 1));
    float m = v - c;

    if (0 <= hPrime && hPrime < 1) {
        r = c;
        g = x;
        b = 0;
    } else if (1 <= hPrime && hPrime < 2) {
        r = x;
        g = c;
        b = 0;
    } else if (2 <= hPrime && hPrime < 3) {
        r = 0;
        g = c;
        b = x;
    } else if (3 <= hPrime && hPrime < 4) {
        r = 0;
        g = x;
        b = c;
    } else if (4 <= hPrime && hPrime < 5) {
        r = x;
        g = 0;
        b = c;
    } else if (5 <= hPrime && hPrime < 6) {
        r = c;
        g = 0;
        b = x;
    } else {
        r = 0;
        g = 0;
        b = 0;
    }

    r += m;
    g += m;
    b += m;
}
}
