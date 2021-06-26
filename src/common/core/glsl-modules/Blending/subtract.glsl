#ifndef ALGINE_MODULE_BLENDING_SUBTRACT
#define ALGINE_MODULE_BLENDING_SUBTRACT

float blendSubtract(float base, float blend) {
	return max(base + blend - 1.0f, 0.0f);
}

vec3 blendSubtract(vec3 base, vec3 blend) {
	return max(base + blend - vec3(1.0f), vec3(0.0f));
}

vec3 blendSubtract(vec3 base, vec3 blend, float opacity) {
	return (blendSubtract(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_SUBTRACT
