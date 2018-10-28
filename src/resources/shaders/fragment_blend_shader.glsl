/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330

#algdef

out vec4 fragColor;
in vec2 texCoord;

#ifdef ALGINE_BLOOM_MODE_ENABLED
uniform sampler2D bloomScene;
#endif

uniform sampler2D dofScene;
uniform float exposure;
uniform float gamma;

void main() {
	fragColor = 
		vec4(
			pow(
				vec3(1.0) - exp(-
					(
						#ifdef ALGINE_BLOOM_MODE_ENABLED
						texture(bloomScene, texCoord).rgb +
						#endif 
						
						texture(dofScene, texCoord).rgb // additive blending
					) 
				* exposure), // tone mapping
			vec3(1.0 / gamma)), // also gamma correct while we’re at it
		1.0);
}
