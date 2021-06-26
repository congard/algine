#ifndef ALGINE_MODULE_BLENDING_NEGATION
#define ALGINE_MODULE_BLENDING_NEGATION

vec3 blendNegation(vec3 base, vec3 blend) {
	return vec3(1.0f) - abs(vec3(1.0f) - base - blend);
}

vec3 blendNegation(vec3 base, vec3 blend, float opacity) {
	return (blendNegation(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_NEGATION
