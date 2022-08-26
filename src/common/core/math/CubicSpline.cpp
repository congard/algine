#include <algine/core/math/CubicSpline.h>
#include <cmath>
#include <glm/common.hpp>

// Based on https://stackoverflow.com/a/19216702/9200394
// and https://en.wikipedia.org/wiki/Spline_(mathematics)

namespace algine {
CubicSpline::CubicSpline() = default;

CubicSpline::CubicSpline(const std::vector<glm::vec2> &points) {
    setPoints(points);
}

void CubicSpline::setPoints(const std::vector<glm::vec2> &points) {
    m_points = points;

    // gen spline
    const auto &v = m_points;
    int n = static_cast<int>(v.size()) - 1;

    std::vector<float> b(n);
    std::vector<float> d(n);
    std::vector<float> h;

    for (int i = 0; i < n; ++i)
        h.push_back(v[i + 1].x - v[i].x);

    std::vector<float> alpha;
    alpha.push_back(0);

    for (int i = 1; i < n; ++i)
        alpha.push_back(3 * (v[i + 1].y - v[i].y) / h[i] - 3 * (v[i].y - v[i - 1].y) / h[i - 1]);

    std::vector<float> c(n + 1);
    std::vector<float> l(n + 1);
    std::vector<float> mu(n + 1);
    std::vector<float> z(n + 1);

    l[0] = 1;
    mu[0] = 0;
    z[0] = 0;

    for (int i = 1; i < n; ++i) {
        l[i] = 2 * (v[i + 1].x - v[i - 1].x) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }

    l[n] = 1;
    z[n] = 0;
    c[n] = 0;

    for (int j = n - 1; j >= 0; --j) {
        c[j] = z[j] - mu[j] * c[j + 1];
        b[j] = (v[j + 1].y - v[j].y) / h[j] - h[j] * (c[j + 1] + 2 * c[j]) / 3;
        d[j] = (c[j + 1] - c[j]) / 3 / h[j];
    }

    m_spline.resize(n);

    for (int i = 0; i < n; ++i) {
        m_spline[i].b = b[i];
        m_spline[i].c = c[i];
        m_spline[i].d = d[i];
    }
}

const std::vector<glm::vec2>& CubicSpline::getPoints() const {
    return m_points;
}

float CubicSpline::getY(float x) const {
    if (x < m_points.front().x || x > m_points.back().x)
        return NAN;

    // 1 -- 2 -- 3 -- 4 -- 5
    // * -- * -- * -- * --
    // SplineSet exists only for points marked as (*)

    // x_j: max(x_j), x_j <= x
    // S_j(x) = a_j + b_j * (x - x_j) + c_j * (x - x_j)^2 + d_j * (x - x_j)^3
    // a_j = y_j

    // Example: If we want to calculate y(1.5), take SplineSet of x=1
    // i.e. m_spline[0] and calculate using the above formula

    int j = 0;
    int n = static_cast<int>(m_points.size());

    if (n < 128) { // linear search
        while (j < n && m_points[j].x <= x) ++j;
        --j;
    } else { // binary search
        int low = 0;
        int high = n - 1;

        while (low <= high) {
            j = low + (high - low) / 2;

            if (x < m_points[j].x) {
                high = j - 1;
            } else if (x >= m_points[j + 1].x) {
                low = j + 1;
            } else {
                break;
            }
        }
    }

    if (m_points[j].x == x)
        return m_points[j].y;

    auto &sp = m_spline[j];
    float delta = x - m_points[j].x;

    return m_points[j].y + sp.b * delta + sp.c * powf(delta, 2) + sp.d * powf(delta, 3);
}

glm::vec2 CubicSpline::getPoint(float t) const {
    if (t < 0 || t > 1)
        return {NAN, NAN};
    float x = glm::mix(m_points.front().x, m_points.back().x, t);
    return {x, getY(x)};
}
} // algine
