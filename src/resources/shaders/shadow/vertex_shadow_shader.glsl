/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * github.com/congard
 */

#version 330 core

in vec4 a_BoneWeights[MAX_BONE_ATTRIBS_PER_VERTEX];
in ivec4 a_BoneIds[MAX_BONE_ATTRIBS_PER_VERTEX];
in vec4 a_Position;

// if dif light transformationMatrix = lightSpaceMatrix * modelMatrix
// if point light transformationMatrix = modelMatrix
uniform mat4 transformationMatrix;
uniform mat4 bones[MAX_BONES];
uniform int boneAttribsPerVertex = 0;

void main() {
	vec4 position = a_Position;

	#ifdef ALGINE_BONE_SYSTEM_ENABLED
    if (boneAttribsPerVertex != 0) {
        mat4 finalTransform = mat4(0.0);
        for (int i = 0; i < boneAttribsPerVertex; i++) {
            finalTransform += bones[a_BoneIds[i].x] * a_BoneWeights[i].x;
            finalTransform += bones[a_BoneIds[i].y] * a_BoneWeights[i].y;
            finalTransform += bones[a_BoneIds[i].z] * a_BoneWeights[i].z;
            finalTransform += bones[a_BoneIds[i].w] * a_BoneWeights[i].w;
        }

        position = finalTransform * position;
    }
    #endif

	gl_Position = transformationMatrix * position;
}
