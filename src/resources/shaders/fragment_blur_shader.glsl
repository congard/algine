/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330

#algdef

in vec2 texCoord;

#ifdef ALGINE_BLOOM_MODE_ENABLED
layout (location = 0) out vec4 bloomFragColor;
uniform sampler2D image; // bloom
uniform float bloom_kernel[BLOOM_KERNEL_SIZE];
vec3 bloom_result;
#endif

#ifdef ALGINE_DOF_MODE_ENABLED
layout (location = 1) out vec4 dofFragColor;

uniform sampler2D scene; // dof
uniform sampler2D dofBuffer;
uniform float max_sigma;
uniform float min_sigma;

float dof_kernel[DOF_KERNEL_SIZE];

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
#endif

void main() {
	#ifdef ALGINE_BLOOM_MODE_ENABLED
	vec2 texOffset = 1.0 / textureSize(image, 0); // gets size of single texel
	#else
	vec2 texOffset = 1.0 / textureSize(scene, 0); // gets size of single texel
	#endif

	#ifdef ALGINE_DOF_MODE_ENABLED
	makeDofKernel(max_sigma * texture(dofBuffer, texCoord).r + min_sigma);
	dofFragColor = texture(scene, texCoord).rgba * dof_kernel[0];
	#endif
	
	#ifdef ALGINE_BLOOM_MODE_ENABLED
	bloom_result = texture(image, texCoord).rgb * bloom_kernel[0]; // current fragment’s contribution
	#endif

	#ifdef ALGINE_BLUS_HORIZONTAL
	#ifdef ALGINE_BLOOM_MODE_ENABLED
	for(int i = 1; i < BLOOM_KERNEL_SIZE; i++) {
		bloom_result +=
			bloom_kernel[i] * (
				texture(image, texCoord + vec2(texOffset.x * i, 0.0)).rgb +
				texture(image, texCoord - vec2(texOffset.x * i, 0.0)).rgb
			);
	}
	#endif

	#ifdef ALGINE_DOF_MODE_ENABLED
	for(int i = 1; i < DOF_KERNEL_SIZE; i++) {
		dofFragColor +=
			dof_kernel[i] * (
				texture(scene, texCoord + vec2(texOffset.x * i, 0.0)).rgba +
				texture(scene, texCoord - vec2(texOffset.x * i, 0.0)).rgba
			);
	}
	#endif
	#else
	#ifdef ALGINE_BLOOM_MODE_ENABLED
	for(int i = 1; i < BLOOM_KERNEL_SIZE; i++) {
		bloom_result +=
			bloom_kernel[i] * (
				texture(image, texCoord + vec2(0.0, texOffset.y * i)).rgb +
				texture(image, texCoord - vec2(0.0, texOffset.y * i)).rgb
			);
	}
	#endif

	#ifdef ALGINE_DOF_MODE_ENABLED
	for(int i = 1; i < DOF_KERNEL_SIZE; i++) {
		dofFragColor +=
			dof_kernel[i] * (
				texture(scene, texCoord + vec2(0.0, texOffset.y * i)).rgba +
				texture(scene, texCoord - vec2(0.0, texOffset.y * i)).rgba
			);
	}
	#endif
	#endif

	#ifdef ALGINE_BLOOM_MODE_ENABLED
	bloomFragColor = vec4(bloom_result, 1.0);
	#endif
}
