in vec2 texCoord;

layout (location = 0) out float fragCoC;

uniform sampler2D positionMap;

uniform struct Linear {
	float maxSigma;
	float minSigma;
	float focalDepth;
	float focalRange;
} linear;

uniform struct Cinematic {
	float p; // plane in focus
	float a; // aperture
	float i; // image distance
} cinematic;

void main() {
    #ifdef ALGINE_CINEMATIC_DOF
		float p = -cinematic.p;
		float f = (p + cinematic.i) / (p * cinematic.i);
		float d = -texture(positionMap, texCoord).z;
		float sigma = abs((cinematic.a * f * (p - d)) / (d * (p - f)));
		fragCoC = sigma;
	#else
		float sigma = abs(linear.focalDepth + texture(positionMap, texCoord).z) / linear.focalRange;
		fragCoC = linear.maxSigma * sigma + linear.minSigma;
	#endif
}
