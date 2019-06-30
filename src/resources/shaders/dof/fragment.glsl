// Algine dof blur fragment shader

/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * github.com/congard
 */

#version 330

#algdef

in vec2 texCoord;

layout (location = 0) out vec3 fragColor;

uniform sampler2D image;
uniform sampler2D positionMap;
uniform sampler2D cocMap;

// if ALGINE_DOF_MODE == ALGINE_DOF_MODE_ENABLED
uniform float max_sigma;
uniform float min_sigma;
uniform float focalDepth;
uniform float focalRange;

// if ALGINE_DOF_MODE == ALGINE_CINEMATIC_DOF_MODE_ENABLED
uniform struct CinematicDOF {
	float p; // plane in focus
	float a; // aperture
	float i; // image distance
} cinematicDOF;

uniform float bleedingMinDeltaZ;
uniform float bleedingMinDeltaCoC;
uniform float bleedingMaxFocusCoC;

float dof_kernel[KERNEL_RADIUS];

const int DOF_LCR_SIZE = KERNEL_RADIUS * 2 - 1; // left-center-right (lllcrrr)
const int DOF_MEAN = DOF_LCR_SIZE / 2;

float coc;
float zCenter; // for eliminate bleeding artifact

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

float getCoC(vec2 texCoord) {
	#ifdef ALGINE_CINEMATIC_DOF
		float p = -cinematicDOF.p;
		float f = (p + cinematicDOF.i) / (p * cinematicDOF.i);
		float d = -texture(positionMap, texCoord).z;
		float sigma = abs((cinematicDOF.a * f * (p - d)) / (d * (p - f)));
		return sigma;
	#elif defined ALGINE_LINEAR_DOF
		float sigma = abs(focalDepth + texture(positionMap, texCoord).z) / focalRange;
		return max_sigma * sigma + min_sigma;
	#else
		return texture(cocMap, texCoord).r;
	#endif
}

bool isBleeding(vec2 texCoord) {
	float offsetCoC = getCoC(texCoord);

	bool bleeding = true;

	#ifdef ALGINE_BLEEDING_ELIM_FCOC
		bleeding = bleeding && (offsetCoC <= bleedingMaxFocusCoC);
	#endif

	#ifdef ALGINE_BLEEDING_ELIM_DZ
		bleeding = bleeding && (zCenter - texture(positionMap, texCoord).z <= -bleedingMinDeltaZ);
	#endif

	#ifdef ALGINE_BLEEDING_ELIM_DCOC
		bleeding = bleeding && (coc - offsetCoC >= bleedingMinDeltaCoC);
	#endif

	return bleeding;
}

void main() {
	vec2 texOffset = 1.0 / textureSize(image, 0); // gets size of single texel

	coc = getCoC(texCoord);
	makeDofKernel(coc);

	vec3 colorCenter = texture(image, texCoord).rgb;
	zCenter = texture(positionMap, texCoord).z; // for eliminate bleeding artifact

	fragColor = colorCenter * dof_kernel[0];

	vec3 colorP, colorM; // "color plus" and "color minus"
	vec2 texCoordP, texCoordM; // "texCoord plus" and "texCoord minus"
	
	#ifdef ALGINE_HORIZONTAL
		#define _texCoordP texCoord + vec2(texOffset.x * i, 0.0)
		#define _texCoordM texCoord - vec2(texOffset.x * i, 0.0)
	#else
		#define _texCoordP texCoord + vec2(0.0, texOffset.y * i)
		#define _texCoordM texCoord - vec2(0.0, texOffset.y * i)
	#endif

	for(int i = 1; i < KERNEL_RADIUS; i++) {
		texCoordP = _texCoordP;
		texCoordM = _texCoordM;
		colorP = texture(image, texCoordP).rgb;
		colorM = texture(image, texCoordM).rgb;

		#if defined ALGINE_BLEEDING_ELIM_DZ || defined ALGINE_BLEEDING_ELIM_DCOC || defined ALGINE_BLEEDING_ELIM_FCOC
			if (isBleeding(texCoordP)) colorP = colorCenter;
			if (isBleeding(texCoordM)) colorM = colorCenter;
		#endif
			
		fragColor += dof_kernel[i] * (colorP + colorM);
	}
}