/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330

#define BLOOM_KERNEL_SIZE 8
#define DOF_KERNEL_SIZE 8
/* ^^^ definitions ^^^ */

layout (location = 0) out vec4 bloomFragColor;
layout (location = 1) out vec4 dofFragColor;

in vec2 texCoords;

uniform sampler2D image; // bloom
uniform sampler2D image2; // dof
uniform bool isHorizontal;

uniform float max_sigma;
uniform float min_sigma;

uniform float kernel[BLOOM_KERNEL_SIZE];

float dof_kernel[DOF_KERNEL_SIZE];

vec4 tmp;
vec3 bloom_result;
vec3 dof_result;
float fdof;

const int DOF_LCR_SIZE = DOF_KERNEL_SIZE * 2 - 1; // left-center-right (lllcrrr)
const int DOF_MEAN = DOF_LCR_SIZE / 2;

void makeDofKernel(float sigma) {
	float sum = 0; // For accumulating the kernel values
	for (int x = DOF_MEAN; x < DOF_LCR_SIZE; x++)  {
		dof_kernel[x - DOF_MEAN] = exp(-0.5 * pow((x - DOF_MEAN) / sigma, 2.0));
	    // Accumulate the kernel values
	    sum += dof_kernel[x - DOF_MEAN];
	}

	sum += sum - dof_kernel[0];

	// Normalize the kernel
	for (int x = 0; x < DOF_KERNEL_SIZE; x++) dof_kernel[x] /= sum;
}

void main() {
	vec2 texOffset = 1.0 / textureSize(image, 0); // gets size of single texel
	tmp = texture(image2, texCoords);
	fdof = tmp.a;
	makeDofKernel(max_sigma * fdof + min_sigma);
	
	bloom_result = texture(image, texCoords).rgb * kernel[0]; // current fragment’s contribution
	dof_result = tmp.rgb * dof_kernel[0];

	if(isHorizontal) {
		for(int i = 1; i < BLOOM_KERNEL_SIZE; i++) {
			bloom_result +=
				kernel[i] * (
					texture(image, texCoords + vec2(texOffset.x * i, 0.0)).rgb +
					texture(image, texCoords - vec2(texOffset.x * i, 0.0)).rgb
				);
		}

		for(int i = 1; i < DOF_KERNEL_SIZE; i++) {
			dof_result +=
				dof_kernel[i] * (
					texture(image2, texCoords + vec2(texOffset.x * i, 0.0)).rgb +
					texture(image2, texCoords - vec2(texOffset.x * i, 0.0)).rgb
				);
		}
	} else {
		for(int i = 1; i < BLOOM_KERNEL_SIZE; i++) {
			bloom_result +=
				kernel[i] * (
					texture(image, texCoords + vec2(0.0, texOffset.y * i)).rgb +
					texture(image, texCoords - vec2(0.0, texOffset.y * i)).rgb
				);
		}

		for(int i = 1; i < DOF_KERNEL_SIZE; i++) {
			dof_result +=
				dof_kernel[i] * (
					texture(image2, texCoords + vec2(0.0, texOffset.y * i)).rgb +
					texture(image2, texCoords - vec2(0.0, texOffset.y * i)).rgb
				);
		}
	}

	bloomFragColor = vec4(bloom_result, 1.0);
	dofFragColor = vec4(dof_result, fdof);
}
