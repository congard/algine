#ifndef ALGINE_MODULE_BLENDING_HARD_LIGHT
#define ALGINE_MODULE_BLENDING_HARD_LIGHT

#alp include <Blending/overlay>

vec3 blendHardLight(vec3 base, vec3 blend) {
	return blendOverlay(blend, base);
}

vec3 blendHardLight(vec3 base, vec3 blend, float opacity) {
	return (blendHardLight(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_HARD_LIGHT
