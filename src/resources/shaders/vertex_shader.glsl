/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard

  COMPILER WILL REMOVE UNUSED VARIABLES
 */

#version 330

#algdef

uniform mat4 u_PVMMatrix, u_Model, u_View, u_VMMatrix;
uniform float u_NormalMapping;  // Normal mapping; 0 - false, 1 - true

in vec4 a_Position; // Per-vertex position information we will pass in.
in vec3 a_Normal, a_Tangent, a_Bitangent;
in vec2 a_Texture; // Per-vertex texture information we will pass in.

out mat4 model, view, vmMatrix;
out mat3 v_TBN;
out vec4 v_Position;
out vec3 v_Normal;
out vec2 v_Texture;
out float v_NormalMapping;

void main() {
    #ifdef ALGINE_NORMAL_MAPPING_MODE_DUAL
	// creating TBN (tangent-bitangent-normal) matrix if normal mapping enabled
	if (u_NormalMapping == 1) {
		vec3 T = normalize(vec3(u_VMMatrix * vec4(a_Tangent, 0.0))); // tangent
		vec3 B = normalize(vec3(u_VMMatrix * vec4(a_Bitangent, 0.0))); // bitangent
		vec3 N = normalize(vec3(u_VMMatrix * vec4(a_Normal, 0.0))); // normal
		mat3 TBN = mat3(T, B, N);
		v_TBN = TBN;
	}
    v_NormalMapping = u_NormalMapping;
    #elif defined ALGINE_NORMAL_MAPPING_MODE_ENABLED
	// creating TBN (tangent-bitangent-normal) matrix if normal mapping enabled
	vec3 T = normalize(vec3(u_VMMatrix * vec4(a_Tangent, 0.0))); // tangent
	vec3 B = normalize(vec3(u_VMMatrix * vec4(a_Bitangent, 0.0))); // bitangent
	vec3 N = normalize(vec3(u_VMMatrix * vec4(a_Normal, 0.0))); // normal
	mat3 TBN = mat3(T, B, N);
	v_TBN = TBN;
    #endif

    // gl_Position is a special variable used to store the final position.
    // Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
    gl_Position = u_PVMMatrix * a_Position;

    // sending all needed variables to fragment shader
    v_Position = a_Position;
	v_Normal = a_Normal;
    v_Texture = a_Texture;
    model = u_Model;
    view = u_View;
	vmMatrix = u_VMMatrix;
}