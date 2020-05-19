#ifndef ALGINE_MATHTOOLS_H
#define ALGINE_MATHTOOLS_H

#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

namespace algine {
namespace MathTools {
/// Writes gaussian blur kernel in <i>kernel</i> array
void getBlurKernel(float *kernel, int size, float sigma);

/**
 * @param radians angle in radians
 * @param rotationAxis axis around which rotation will be applied
 */
glm::quat getQuat(float radians, const glm::vec3 &rotationAxis);
}
}

#endif