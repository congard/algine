/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My github: https://github.com/congard
 * @author congard
 */
#include <tgmath.h>
#include <algine/std/math.h>

namespace algine {
/**
 * Calculating one dimensional Gaussian Blur kernel
 * @param size - kernel size
 * @param sigma - sigma value
 * @return pointer to 1D kernel
 */
void getGB1DKernel(float *kernel, int size, float sigma) {
	int mean = size / 2;
	float sum = 0; // For accumulating the kernel values
	for (int x = 0; x < size; x++)  {
	    kernel[x] = exp(-0.5 * pow((x - mean) / sigma, 2.0));
	    // Accumulate the kernel values
	    sum += kernel[x];
	}

	// Normalize the kernel
	for (int x = 0; x < size; x++) 
        kernel[x] /= sum;
}

/**
 * radians it is angle in radians
 * rotationAxis it is the axis around which you want to make rotation
 */
glm::quat getQuat(float radians, const glm::vec3 &rotationAxis) {
	return glm::quat(
		cos(radians / 2),					// w
		rotationAxis.x * sin(radians / 2),	// x
		rotationAxis.y * sin(radians / 2),	// y
		rotationAxis.z * sin(radians / 2)	// z
	);
}

// converts aiMatrix4x4 to glm::mat4
glm::mat4 getMat4(const aiMatrix4x4 &in_mat) {
    glm::mat4 mat;
    
    mat[0][0] = in_mat.a1;
    mat[1][0] = in_mat.a2;
    mat[2][0] = in_mat.a3;
    mat[3][0] = in_mat.a4;

    mat[0][1] = in_mat.b1;
    mat[1][1] = in_mat.b2;
    mat[2][1] = in_mat.b3;
    mat[3][1] = in_mat.b4;

    mat[0][2] = in_mat.c1;
    mat[1][2] = in_mat.c2;
    mat[2][2] = in_mat.c3;
    mat[3][2] = in_mat.c4;

    mat[0][3] = in_mat.d1;
    mat[1][3] = in_mat.d2;
    mat[2][3] = in_mat.d3;
    mat[3][3] = in_mat.d4;
    
    return mat;
}

} /* namespace algine */