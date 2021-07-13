#ifndef ALGINE_SCALABLE_H
#define ALGINE_SCALABLE_H

#include <algine/core/transfer/Transferable.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace algine {
class Scalable: public Transferable {
public:
    Scalable();

    void changeScale(const glm::vec3 &dScale);

    void setScale(float x, float y, float z);
    void setScale(const glm::vec3 &scale);
    void setScaleX(float x);
    void setScaleY(float y);
    void setScaleZ(float z);
    void scale();

    float getScaleX() const;
    float getScaleY() const;
    float getScaleZ() const;
    glm::vec3 getScale() const;
    glm::mat4 getScalingMatrix() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

public:
    glm::vec3 m_scale;
    glm::mat4 m_scaling;
};
}

#endif //ALGINE_SCALABLE_H
