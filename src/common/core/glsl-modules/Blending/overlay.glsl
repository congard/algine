#ifndef ALGINE_MODULE_BLENDING_OVERLAY
#define ALGINE_MODULE_BLENDING_OVERLAY

float blendOverlay(float base, float blend) {
	return base < 0.5f ? (2.0f * base * blend) : (1.0f - 2.0f * (1.0f - base) * (1.0f - blend));
}

vec3 blendOverlay(vec3 base, vec3 blend) {
	return vec3(blendOverlay(base.r, blend.r), blendOverlay(base.g, blend.g), blendOverlay(base.b, blend.b));
}

vec3 blendOverlay(vec3 base, vec3 blend, float opacity) {
	return (blendOverlay(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_OVERLAY
