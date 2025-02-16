#ifndef ALGINE_MODULE_BLENDING_PHOENIX
#define ALGINE_MODULE_BLENDING_PHOENIX

vec3 blendPhoenix(vec3 base, vec3 blend) {
	return min(base, blend) - max(base, blend) + vec3(1.0f);
}

vec3 blendPhoenix(vec3 base, vec3 blend, float opacity) {
	return (blendPhoenix(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_PHOENIX
