#version 330

layout (location = 0) in vec3 inPos;
out vec2 texCoord;

void main() {
	texCoord = inPos.xy * 0.5 + 0.5;
	gl_Position = vec4(inPos, 1.0);
}