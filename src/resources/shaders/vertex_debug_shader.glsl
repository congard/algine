/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * github.com/congard
 *
 * This shader need only for DEBUGGING!
 */

#version 130

attribute vec3 a_Position;
attribute vec2 a_TexCoord;
varying vec2 v_TexCoord;

void main() {
	v_TexCoord = a_TexCoord;
	gl_Position = vec4(a_Position, 1.0);
}
