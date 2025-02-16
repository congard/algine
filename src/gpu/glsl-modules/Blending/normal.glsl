#ifndef ALGINE_MODULE_BLENDING_NORMAL
#define ALGINE_MODULE_BLENDING_NORMAL

vec3 blendNormal(vec3 base, vec3 blend) {
	return blend;
}

vec3 blendNormal(vec3 base, vec3 blend, float opacity) {
	return (blendNormal(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_NORMAL
