#version 330 core

// #define vecout vec3

in vec3 texCoords;

layout(location = ALGINE_CUBEMAP_COLOR_OUT_COLOR_COMPONENT) out vecout fragColor;
layout(location = ALGINE_POS_OUT_COLOR_COMPONENT) out vec3 fragPos;

uniform mat3 view; // need if you want write positions

uniform samplerCube cubemap;
uniform vecout color = vecout(1.0);
uniform float scaling = 1.0;

void main() {
    // if defined output color component then color rendering enabled
    #ifdef ALGINE_CUBEMAP_COLOR_OUT_COLOR_COMPONENT
        fragColor = vecout(texture(cubemap, texCoords)) * color;
    #endif

    #ifdef ALGINE_CUBE_POSITIONS
        fragPos = vecout(view * texCoords) * scaling;
    #elif defined ALGINE_SPHERE_POSITIONS
        fragPos = vecout(normalize(view * texCoords)) * scaling;
    #endif
}
