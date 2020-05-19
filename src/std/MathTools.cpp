#include <ctgmath>
#include <algine/std/MathTools.h>

namespace algine {
namespace MathTools {
void getBlurKernel(float *kernel, int size, float sigma) {
	int mean = size / 2;
	float sum = 0; // For accumulating the kernel values
	for (int x = 0; x < size; x++)  {
	    kernel[x] = expf(-0.5f * powf((float)(x - mean) / sigma, 2.0));
	    // Accumulate the kernel values
	    sum += kernel[x];
	}

	// Normalize the kernel
	for (int x = 0; x < size; x++)
        kernel[x] /= sum;
}

glm::quat getQuat(float radians, const glm::vec3 &rotationAxis) {
	return glm::quat(
		cosf(radians / 2),					// w
		rotationAxis.x * sinf(radians / 2),	// x
		rotationAxis.y * sinf(radians / 2),	// y
		rotationAxis.z * sinf(radians / 2)	// z
	);
}
}
} // namespace algine