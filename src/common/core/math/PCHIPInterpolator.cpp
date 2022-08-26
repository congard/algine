#include <algine/core/math/PCHIPInterpolator.h>
#include <cstdlib>
#include <cmath>
#include <glm/common.hpp>

// Based on https://github.com/slabanja/SimplePCHIP
// which is written in Julia and licensed under the Simplified "2-clause" BSD License:
// Copyright (c) 2017: mattias slabanja. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

namespace algine {
PCHIPInterpolator::PCHIPInterpolator() = default;

PCHIPInterpolator::PCHIPInterpolator(const std::vector<glm::vec2> &points) {
    setPoints(points);
}

template <typename T>
inline int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

template<typename T>
inline bool approxZero(T n) {
    constexpr T epsilon = 10E-8;
    return std::abs(n) <= epsilon;
}

void PCHIPInterpolator::setPoints(const std::vector<glm::vec2> &points) {
    m_points = points;

    // init interpolator
    auto edgeDerivative = [](float h1, float h2, float d1, float d2) {
        float d = ((2 * h1 + h2) * d1 - h2 * d2) / (h1 + h2);

        if (sign(d) != sign(d1))
            return 0.0f;

        if (sign(d1) != sign(d2)  &&  std::abs(d) > std::abs(3 * d1))
            return 3 * d1;

        return d;
    };

    auto h = [&](int i) { return points[i + 1].x - points[i].x; };
    auto d = [&](int i) { return (points[i + 1].y - points[i].y) / h(i); };

    int n = static_cast<int>(points.size());
    m_ds.resize(n);

    if (n == 2) {
        std::fill(m_ds.begin(), m_ds.end(), d(0));
    } else {
        float dl = d(0);
        float hl = h(0);

        for (int i = 1; i < n - 1; ++i) {
            float dr = d(i);
            float hr = h(i);

            if (sign(dl) != sign(dr) || approxZero(dl) || approxZero(dr)) {
                m_ds[i] = 0.0f;
            } else {
                float wl = 2 * hl + hr;
                float wr = hl + 2 * hr;
                m_ds[i] = (wl + wr) / (wl / dl + wr / dr);
            }

            dl = dr;
            hl = hr;
        }

        m_ds[0] = edgeDerivative(h(0), h(1), d(0), d(1));
        m_ds[n - 1] = edgeDerivative(h(n - 2), h(n - 3), d(n - 2), d(n - 3));
    }
}

const std::vector<glm::vec2>& PCHIPInterpolator::getPoints() const {
    return m_points;
}

float PCHIPInterpolator::getY(float x) const {
    if (x < m_points.front().x || x > m_points.back().x)
        return NAN;

    int n = static_cast<int>(m_points.size());
    int i = 0;

    if (n < 128) { // linear search
        while (i < n - 1 && x >= m_points[i + 1].x) ++i;
    } else { // binary search
        int low = 0;
        int high = n - 1;

        while (low <= high) {
            i = low + (high - low) / 2;

            if (x < m_points[i].x) {
                high = i - 1;
            } else if (x >= m_points[i + 1].x) {
                low = i + 1;
            } else {
                break;
            }
        }
    }

    // Treat right endpoint as part of rightmost interval
    if (i == n - 1)
        --i;

    float x1 = m_points[i].x;
    float x2 = m_points[i + 1].x;
    float y1 = m_points[i].y;
    float y2 = m_points[i + 1].y;
    float d1 = m_ds[i];
    float d2 = m_ds[i + 1];
    float h = x2 - x1;

    auto phi = [](float t) { return 3 * powf(t, 2) - 2 * powf(t, 3); };
    auto psi = [](float t) { return powf(t, 3) - powf(t, 2); };

    return y1 * phi((x2 - x) / h) + y2 * phi((x - x1) / h) - d1*h * psi((x2 - x) / h) + d2 * h * psi((x - x1) / h);
}

glm::vec2 PCHIPInterpolator::getPoint(float t) const {
    if (t < 0 || t > 1)
        return {NAN, NAN};
    float x = glm::mix(m_points.front().x, m_points.back().x, t);
    return {x, getY(x)};
}
} // algine
