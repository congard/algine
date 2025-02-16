#ifndef ALGINE_MODULE_BLENDING_LINEAR_DODGE
#define ALGINE_MODULE_BLENDING_LINEAR_DODGE

float blendLinearDodge(float base, float blend) {
	// Note : Same implementation as BlendAddf
	return min(base + blend, 1.0f);
}

vec3 blendLinearDodge(vec3 base, vec3 blend) {
	// Note : Same implementation as BlendAdd
	return min(base + blend, vec3(1.0f));
}

vec3 blendLinearDodge(vec3 base, vec3 blend, float opacity) {
	return (blendLinearDodge(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_LINEAR_DODGE
