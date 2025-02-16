#ifndef ALGINE_MODULE_BLENDING_SOFT_LIGHT
#define ALGINE_MODULE_BLENDING_SOFT_LIGHT

float blendSoftLight(float base, float blend) {
	return (blend < 0.5f) ? (2.0f * base * blend + base * base * (1.0f - 2.0f * blend)) : (sqrt(base) * (2.0f * blend - 1.0f) + 2.0f * base * (1.0f - blend));
}

vec3 blendSoftLight(vec3 base, vec3 blend) {
	return vec3(blendSoftLight(base.r, blend.r), blendSoftLight(base.g, blend.g), blendSoftLight(base.b, blend.b));
}

vec3 blendSoftLight(vec3 base, vec3 blend, float opacity) {
	return (blendSoftLight(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_SOFT_LIGHT
