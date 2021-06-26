#ifndef ALGINE_MODULE_BLENDING_SCREEN
#define ALGINE_MODULE_BLENDING_SCREEN

float blendScreen(float base, float blend) {
	return 1.0f - ((1.0f - base) * (1.0f - blend));
}

vec3 blendScreen(vec3 base, vec3 blend) {
	return vec3(blendScreen(base.r, blend.r), blendScreen(base.g, blend.g), blendScreen(base.b, blend.b));
}

vec3 blendScreen(vec3 base, vec3 blend, float opacity) {
	return (blendScreen(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_SCREEN
