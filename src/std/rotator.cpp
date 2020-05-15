#define GLM_FORCE_CTOR_INIT

#include <algine/std/rotator.h>
#include <iostream>

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
        std::cerr << "Unknown rotator type\n";
        return nullptr;
    }
}

EulerRotator::EulerRotator() {
    type = RotatorTypeEuler;
}

void EulerRotator::rotate(glm::mat4 &matrix) {
    glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat qRoll = glm::angleAxis(roll, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::quat orientation = glm::normalize(qPitch * qYaw * qRoll);
    matrix = glm::mat4_cast(orientation);
}

FreeRotator::FreeRotator() {
    type = RotatorTypeFree;
}

void FreeRotator::rotate(glm::mat4 &matrix) {
    // temporary frame quaternion from pitch, yaw, roll
    glm::quat qPYR = glm::quat(glm::vec3(pitch, yaw, roll));
    // reset values
    pitch = yaw = roll = 0;

    // update qRotation
    qRotation = glm::normalize(qPYR * qRotation);
    matrix = glm::mat4_cast(qRotation);
}
}
