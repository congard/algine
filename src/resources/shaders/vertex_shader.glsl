/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * gitlab.com/congard
 */

#version 130

uniform mat4 u_Model, u_View, u_Projection;
uniform float u_NormalMapping;  // Normal mapping; 0 - false, 1 - true

in vec4 a_Position;     // Per-vertex position information we will pass in.
in vec3 a_Normal;       // Per-vertex normal information we will pass in.
in vec3 a_Tangent;	   // Per-vertex tangent information we will pass in.
in vec3 a_Bitangent;	   // Per-vertex bitangent information we will pass in.
in vec2 a_Texture;	   // Per-vertex texture information we will pass in.

out mat4 model, view, projection;
out mat3 v_TBN;			   // This will be passed into the fragment shader.
out vec4 v_Position;       // This will be passed into the fragment shader.
out vec3 v_Normal;         // This will be passed into the fragment shader.
out vec2 v_Texture;		   // This will be passed into the fragment shader.
out float v_NormalMapping;  // This will be passed into the fragment shader.

void main() {
	// creating TBN (tangent-bitangent-normal) matrix if normal mapping enabled
	if (u_NormalMapping == 1) {
		mat4 mvMatrix = u_View * u_Model;
		vec3 T = normalize(vec3(mvMatrix * vec4(a_Tangent, 0.0))); // tangent
		vec3 B = normalize(vec3(mvMatrix * vec4(a_Bitangent, 0.0))); // bitangent
		vec3 N = normalize(vec3(mvMatrix * vec4(a_Normal, 0.0))); // normal
		mat3 TBN = mat3(T, B, N);
		v_TBN = TBN;
	}

    // gl_Position is a special variable used to store the final position.
    // Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
    gl_Position = u_Projection * u_View * u_Model * a_Position;

    // sending all needed variables to fragment shader
    v_Position = a_Position;
    v_Texture = a_Texture;
    v_NormalMapping = u_NormalMapping;
    v_Normal = a_Normal;
    model = u_Model;
    view = u_View;
    projection = u_Projection;
}
