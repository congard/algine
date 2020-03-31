#ifndef ALGINE_MODULE_MATERIAL_GLSL
#define ALGINE_MODULE_MATERIAL_GLSL

uniform struct Material {
	#ifndef ALGINE_MATERIAL_EXCLUDE_DEFAULT
	sampler2D ambient;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	
	vec4 cambient;
	vec4 cdiffuse;
	vec4 cspecular;

	float ambientStrength;
	float diffuseStrength;
	float specularStrength;
	float shininess;
	#endif

	#ifdef ALGINE_MATERIAL_INCLUDE_CUSTOM
	#alp include "Material.custom.glsl"
	#endif
} material;

#endif // ALGINE_MODULE_MATERIAL_GLSL