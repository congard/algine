/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * github.com/congard
 */

#version 330 core

in vec4 inBoneWeights[MAX_BONE_ATTRIBS_PER_VERTEX];
in ivec4 inBoneIds[MAX_BONE_ATTRIBS_PER_VERTEX];
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
            finalTransform += bones[inBoneIds[i].x] * inBoneWeights[i].x;
            finalTransform += bones[inBoneIds[i].y] * inBoneWeights[i].y;
            finalTransform += bones[inBoneIds[i].z] * inBoneWeights[i].z;
            finalTransform += bones[inBoneIds[i].w] * inBoneWeights[i].w;
        }

        position = finalTransform * position;
    }
    #endif

	gl_Position = transformationMatrix * position;
}
