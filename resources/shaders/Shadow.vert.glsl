#pragma algine include "modules/BoneSystem.glsl"

in vec4 a_Position;

// if dif light transformationMatrix = lightSpaceMatrix * modelMatrix
// if point light transformationMatrix = modelMatrix
uniform mat4 transformationMatrix;

void main() {
	vec4 position = a_Position;

	#ifdef ALGINE_BONE_SYSTEM
    if (isBonesPresent()) {
        position = getBoneTransformMatrix() * position;
    }
    #endif

	gl_Position = transformationMatrix * position;
}
