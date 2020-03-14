/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * github.com/congard
 *
 * This shader need only for DEBUGGING!
 */

#version 330

precision mediump float;						// Set the default precision to medium. We don't need as high of a
											// precision in the fragment shader.

uniform sampler2D u_Texture;
varying vec2 v_TexCoord;

layout(location = 0) out vec4 fragColor;

void main() {
	fragColor = texture2D(u_Texture, v_TexCoord);
}
