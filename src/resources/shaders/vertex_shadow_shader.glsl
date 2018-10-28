/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 330 core

in vec3 a_Position;

uniform mat4 u_ModelMatrix;

void main() {
	gl_Position = u_ModelMatrix * vec4(a_Position, 1.0);
}
