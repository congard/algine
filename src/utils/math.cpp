/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
 * @author congard
 */

#ifndef ALGINE_MATH_CPP
#define ALGINE_MATH_CPP

#include <tgmath.h>

/* --- Gaussian Blur Kernel --- */

/**
 * Calculating one dimensional kernel
 * @param size - kernel size
 * @param sigma - sigma value
 * @return pointer to 1D kernel
 */
static void getGB1DKernel(float *kernel, int size, float sigma) {
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

#endif /* ALGINE_MATH_CPP */