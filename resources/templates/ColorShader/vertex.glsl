/*
 * @author Congard
 * dbcongard@gmail.com
 * t.me/congard
 * github.com/congard
 */

#version 330

#alp include <NormalMapping.vs>
#alp include "modules/BoneSystem.glsl"

uniform mat4 MVPMatrix, modelMatrix, viewMatrix, MVMatrix;

in vec4 inPos; // Per-vertex position information we will pass in.
in vec3 inNormal;
in vec3 inTangent;
in vec3 inBitangent;
in vec2 inTexCoord; // Per-vertex texture information we will pass in.

out mat3 matTBN;
out vec3 worldPosition;
out vec3 viewPosition;
out vec2 texCoord;

void main() {
    vec4 position = inPos;
    vec3 normal = inNormal;

    #ifdef ALGINE_BONE_SYSTEM
    if (isBonesPresent()) {
        mat4 finalTransform = getBoneTransformMatrix();
        position = finalTransform * position;
        normal = mat3(finalTransform) * normal;
    }
    #endif

    // gl_Position is a special variable used to store the final position.
    // Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
    gl_Position = MVPMatrix * position;

    // sending all needed variables to fragment shader
    worldPosition = vec3(modelMatrix * position);
    viewPosition = vec3(viewMatrix * vec4(worldPosition, 1.0));
    texCoord = inTexCoord;
    matTBN = getTBNMatrix(MVMatrix, inTangent, inBitangent, normal);
}
