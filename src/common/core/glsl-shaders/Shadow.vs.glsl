#alp include <BoneSystem>

in vec4 a_Position;

// if directional light, then transformationMatrix = lightSpaceMatrix * modelMatrix
// if point light, then transformationMatrix = modelMatrix
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
