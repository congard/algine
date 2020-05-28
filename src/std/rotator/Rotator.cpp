#define GLM_FORCE_CTOR_INIT
#include <algine/std/rotator/Rotator.h>

#include <algine/std/rotator/EulerRotator.h>
#include <algine/std/rotator/FreeRotator.h>

#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>

namespace algine {
void Rotator::rotate(glm::mat4 &matrix) {
    matrix = glm::rotate(matrix, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, roll, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Rotator::setPitch(const float pitch) {
    this->pitch = pitch;
}

void Rotator::setYaw(const float yaw) {
    this->yaw = yaw;
}

void Rotator::setRoll(const float roll) {
    this->roll = roll;
}

float Rotator::getPitch() {
    return pitch;
}

float Rotator::getYaw() {
    return yaw;
}

float Rotator::getRoll() {
    return roll;
}

Rotator* Rotator::create(const uint type) {
    switch (type) {
        case Rotator::RotatorTypeSimple:
            return new Rotator();
        case Rotator::RotatorTypeEuler:
            return new EulerRotator();
        case Rotator::RotatorTypeFree:
            return new FreeRotator();
        default:
            throw std::runtime_error("Unknown rotator type " + std::to_string(type));
    }
}
}
