#ifndef ALGINE_MATH_H
#define ALGINE_MATH_H

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <assimp/matrix4x4.h>

namespace algine {
/**
 * Calculating one dimensional Gaussian Blur kernel
 * @param size - kernel size
 * @param sigma - sigma value
 * @return pointer to 1D kernel
 */
void getGB1DKernel(float *kernel, int size, float sigma);

/**
 * radians it is angle in radians
 * rotationAxis it is the axis around which you want to make rotation
 */
glm::quat getQuat(float radians, const glm::vec3 &rotationAxis);

// converts aiMatrix4x4 to glm::mat4
glm::mat4 getMat4(const aiMatrix4x4 &in_mat);

}

#endif