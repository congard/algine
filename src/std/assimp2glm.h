#ifndef ALGINE_ASSIMP2GLM_H
#define ALGINE_ASSIMP2GLM_H

#include <glm/mat4x4.hpp>
#include <assimp/matrix4x4.h>

inline glm::mat4 getMat4(const aiMatrix4x4 &aiMat) {
    glm::mat4 mat;

    mat[0][0] = aiMat.a1;
    mat[1][0] = aiMat.a2;
    mat[2][0] = aiMat.a3;
    mat[3][0] = aiMat.a4;

    mat[0][1] = aiMat.b1;
    mat[1][1] = aiMat.b2;
    mat[2][1] = aiMat.b3;
    mat[3][1] = aiMat.b4;

    mat[0][2] = aiMat.c1;
    mat[1][2] = aiMat.c2;
    mat[2][2] = aiMat.c3;
    mat[3][2] = aiMat.c4;

    mat[0][3] = aiMat.d1;
    mat[1][3] = aiMat.d2;
    mat[2][3] = aiMat.d3;
    mat[3][3] = aiMat.d4;

    return mat;
}

#endif //ALGINE_ASSIMP2GLM_H
