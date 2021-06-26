#ifndef ALGINE_MODULE_BLENDING_GLOW
#define ALGINE_MODULE_BLENDING_GLOW

#alp include <Blending/reflect>

vec3 blendGlow(vec3 base, vec3 blend) {
	return blendReflect(blend, base);
}

vec3 blendGlow(vec3 base, vec3 blend, float opacity) {
	return (blendGlow(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_GLOW
