/**
 * Normal mapping module
 * Include it in fragment shader
 * Necessary variables:

    sampler2D normalMappingNormalMap;

 * You must link these variables with yours
 */

#ifndef ALGINE_MODULE_NORMALMAPPING_GLSL
#define ALGINE_MODULE_NORMALMAPPING_GLSL

uniform bool u_NormalMapping; // ALGINE_NORMAL_MAPPING_DUAL
in mat3 v_TBN; // Tangent Bitangent Normal matrix

struct NormalMappingVars {
	vec3 viewNormal;
} normalMapping;

void initNormalMapping() {
#ifdef ALGINE_NORMAL_MAPPING_DUAL
	if (u_NormalMapping == 0)
		normalMapping.viewNormal = viewNormal;
	else { // using normal map if normal mapping enabled
		normalMapping.viewNormal = texture2D(normalMappingNormalMap, texCoord).rgb;
		normalMapping.viewNormal = normalize(normalMapping.viewNormal * 2.0 - 1.0); // from [0; 1] to [-1; 1]
		normalMapping.viewNormal = normalize(v_TBN * normalMapping.viewNormal);
	}
#elif defined ALGINE_NORMAL_MAPPING_FROM_MAP
	normalMapping.viewNormal = texture2D(normalMappingNormalMap, texCoord).rgb;
	normalMapping.viewNormal = normalize(normalMapping.viewNormal * 2.0 - 1.0); // from [0; 1] to [-1; 1]
	normalMapping.viewNormal = normalize(v_TBN * normalMapping.viewNormal);
#else
	normalMapping.viewNormal = viewNormal;
#endif /* ALGINE_NORMAL_MAPPING_DUAL */
}

#endif // ALGINE_MODULE_NORMALMAPPING_GLSL