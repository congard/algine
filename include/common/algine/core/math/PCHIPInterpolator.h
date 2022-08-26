#ifndef ALGINE_PCHIPINTERPOLATOR_H
#define ALGINE_PCHIPINTERPOLATOR_H

#include <vector>
#include <glm/vec2.hpp>

namespace algine {
class PCHIPInterpolator {
public:
    PCHIPInterpolator();
    explicit PCHIPInterpolator(const std::vector<glm::vec2> &points);

    void setPoints(const std::vector<glm::vec2> &points);
    const std::vector<glm::vec2>& getPoints() const;

    float getY(float x) const;
    glm::vec2 getPoint(float t) const;

private:
    std::vector<glm::vec2> m_points;
    std::vector<float> m_ds;
};
} // algine

#endif //ALGINE_PCHIPINTERPOLATOR_H
