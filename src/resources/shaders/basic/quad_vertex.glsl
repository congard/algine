#version 330

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoord;
out vec2 texCoord;

void main() {
	texCoord = inTexCoord;
	gl_Position = vec4(inPos, 1.0);
}