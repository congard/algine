#ifndef ALGINE_MODULE_MATERIAL_GLSL
#define ALGINE_MODULE_MATERIAL_GLSL

uniform struct Material {
	sampler2D normal; // normal mapping sampler
	
	sampler2D reflectionStrength; // ssr samplers
	sampler2D jitter;
	
	sampler2D ambient;
	sampler2D diffuse;
	sampler2D specular;
	
	vec4 cambient;
	vec4 cdiffuse;
	vec4 cspecular;

	float ambientStrength;
	float diffuseStrength;
	float specularStrength;
	float shininess;
} material;

#endif // ALGINE_MODULE_MATERIAL_GLSL