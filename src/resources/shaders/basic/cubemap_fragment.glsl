#version 330 core

#algdef

// #define vecout vec3

in vec3 texCoords;

layout(location = ALGINE_CUBEMAP_COLOR_OUT_COLOR_COMPONENT) out vecout fragColor;
layout(location = ALGINE_POS_OUT_COLOR_COMPONENT) out vec3 fragPos;

uniform samplerCube cubemap;
uniform vecout color = vecout(1.0);
uniform float scaling = 1.0;

void main() {
    #ifdef ALGINE_CUBEMAP_COLOR
        fragColor = vecout(texture(cubemap, texCoords)) * color;
    #endif

    #ifdef ALGINE_CUBE_POSITIONS
        fragPos = vecout(texCoords) * scaling;
    #elif defined ALGINE_SPHERE_POSITIONS
        fragPos = vecout(normalize(texCoords)) * scaling;
    #endif
}