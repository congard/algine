#ifndef ALGINE_MODULE_BLENDING_VIVID_LIGHT
#define ALGINE_MODULE_BLENDING_VIVID_LIGHT

#alp include <Blending/color-dodge>
#alp include <Blending/color-burn>

float blendVividLight(float base, float blend) {
	return (blend < 0.5f) ? blendColorBurn(base, (2.0f * blend)) : blendColorDodge(base, (2.0f * (blend - 0.5f)));
}

vec3 blendVividLight(vec3 base, vec3 blend) {
	return vec3(blendVividLight(base.r, blend.r), blendVividLight(base.g, blend.g), blendVividLight(base.b, blend.b));
}

vec3 blendVividLight(vec3 base, vec3 blend, float opacity) {
	return (blendVividLight(base, blend) * opacity + base * (1.0f - opacity));
}

#endif //ALGINE_MODULE_BLENDING_VIVID_LIGHT
