/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330

in vec3 inPos;
in vec2 inTexCoords;
out vec2 texCoords;

void main() {
	texCoords = inTexCoords;
	gl_Position = vec4(inPos, 1.0);
}
