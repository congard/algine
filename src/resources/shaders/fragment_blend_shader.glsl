/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330

out vec4 fragColor;
in vec2 texCoords;
uniform sampler2D scene;
uniform sampler2D bluredScene;
uniform float exposure;
uniform float gamma;

void main() {
	vec3 hdrColor = texture(scene, texCoords).rgb;
	vec3 bloomColor = texture(bluredScene, texCoords).rgb;
	hdrColor += bloomColor; // additive blending
	// tone mapping
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	// also gamma correct while we’re at it
	result = pow(result, vec3(1.0 / gamma));
	fragColor = vec4(result, 1.0);
}
