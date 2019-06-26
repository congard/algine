// Algine dof blur fragment shader

/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330

#algdef

in vec2 texCoord;

layout (location = 0) out vec3 fragColor;

uniform sampler2D image;
uniform sampler2D positionMap;
uniform float max_sigma;
uniform float min_sigma;

uniform float focalDepth; // if ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED
uniform float focalRange; // if ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED

// if ALGINE_DOF_MODE == ALGINE_CINEMATIC_DOF_MODE_ENABLED
uniform struct CinematicDOF {
	float p; // plane in focus
	float a; // aperture
	float i; // image distance
} cinematicDOF;

float dof_kernel[KERNEL_RADIUS];

const int DOF_LCR_SIZE = KERNEL_RADIUS * 2 - 1; // left-center-right (lllcrrr)
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
	for (int x = 0; x < KERNEL_RADIUS; x++) dof_kernel[x] /= sum;
}

void main() {
	vec2 texOffset = 1.0 / textureSize(image, 0); // gets size of single texel
	
	#ifdef ALGINE_CINEMATIC_DOF
		float p = -cinematicDOF.p;
		float f = (p + cinematicDOF.i) / (p * cinematicDOF.i);
		float d = -texture(positionMap, texCoord).z;
		float sigma = abs((cinematicDOF.a * f * (p - d)) / (d * (p - f)));
		makeDofKernel(sigma);
	#else
		float sigma = abs(focalDepth + texture(positionMap, texCoord).z) / focalRange;
		makeDofKernel(max_sigma * sigma + min_sigma);
	#endif

	fragColor = texture(image, texCoord).rgb * dof_kernel[0];
	
	#ifdef ALGINE_HORIZONTAL
		for(int i = 1; i < KERNEL_RADIUS; i++) {
			fragColor +=
				dof_kernel[i] * (
					texture(image, texCoord + vec2(texOffset.x * i, 0.0)).rgb +
					texture(image, texCoord - vec2(texOffset.x * i, 0.0)).rgb
				);
		}
	#else
		for(int i = 1; i < KERNEL_RADIUS; i++) {
			fragColor +=
				dof_kernel[i] * (
					texture(image, texCoord + vec2(0.0, texOffset.y * i)).rgb +
					texture(image, texCoord - vec2(0.0, texOffset.y * i)).rgb
				);
		}
	#endif
}