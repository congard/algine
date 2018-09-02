/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330

out vec4 fragColor;
in vec2 texCoords;
// uniform sampler2D scene; // will be available in 1.2 alpha
uniform sampler2D bluredScene;
uniform sampler2D dofScene;
uniform float exposure;
uniform float gamma;

void main() {
	fragColor = 
		vec4(
			pow(
				vec3(1.0) - exp(-
					(
						texture(bluredScene, texCoords).rgb + texture(dofScene, texCoords).rgb // additive blending
					) 
				* exposure), // tone mapping
			vec3(1.0 / gamma)), // also gamma correct while we’re at it
		1.0);
}
