#ifndef ALGINE_MODULE_NORMALMAPPING_FS
#define ALGINE_MODULE_NORMALMAPPING_FS

vec3 getNormal(in sampler2D normalMap, in vec2 uv, in mat3 mTBN) {
    vec3 normal = texture2D(normalMap, uv).rgb;
	normal = normalize(normal * 2.0 - 1.0); // from [0; 1] to [-1; 1]
	normal = normalize(mTBN * normal);
	return normal;
}

#endif //ALGINE_MODULE_NORMALMAPPING_FS
