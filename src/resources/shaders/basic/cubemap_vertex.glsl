#version 330 core

layout(location = 0) in vec3 inPos;

out vec3 texCoords;

uniform mat4 transform; // view projection matrix in most cases

void main() {
	vec4 pos = transform * vec4(inPos, 1.0);
	gl_Position = pos.xyww; // optimization !! its work with glDepthFunc(GL_LEQUAL);
							// and draw skybox after scene !!!
	
	texCoords = inPos;
}