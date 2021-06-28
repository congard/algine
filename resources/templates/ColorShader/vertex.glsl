#version 330 core

#alp include <NormalMapping.vs>
#alp include <BoneSystem>

uniform mat4 MVPMatrix, modelMatrix, viewMatrix, MVMatrix;

in vec4 inPos;
in vec3 inNormal;
in vec3 inTangent;
in vec3 inBitangent;
in vec2 inTexCoord;

out mat3 matTBN;
out vec3 worldPosition;
out vec3 viewPosition;
out vec2 texCoord;

void main() {
    vec4 position = inPos;
    vec3 normal = inNormal;

    if (isBonesPresent()) {
        mat4 finalTransform = getBoneTransformMatrix();
        position = finalTransform * position;
        normal = mat3(finalTransform) * normal;
    }

    // gl_Position is a special variable used to store the final position.
    // Multiply the vertex by the matrix to get the final point in normalized screen coordinates.
    gl_Position = MVPMatrix * position;

    worldPosition = vec3(modelMatrix * position);
    viewPosition = vec3(viewMatrix * vec4(worldPosition, 1.0));
    texCoord = inTexCoord;
    matTBN = getTBNMatrix(MVMatrix, inTangent, inBitangent, normal);
}
