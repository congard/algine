#ifndef ALGINE_SHADERPROGRAM_H
#define ALGINE_SHADERPROGRAM_H

#include <algine/types.h>
#include <iostream>

#define useShaderProgram(shaderprogram) glUseProgram(shaderprogram)

#define ALGINE_VERTEX_SHADER GL_VERTEX_SHADER
#define ALGINE_FRAGMENT_SHADER GL_FRAGMENT_SHADER
#define ALGINE_GEOMETRY_SHADER GL_GEOMETRY_SHADER

namespace algine {

void getShaderInfoLog(int shader, int type);

void getProgramInfoLog(int program, int type);

struct Shader {
    static uint create(const uint type);

    static void compile(const uint shader, const std::string &source);

    static void destroy(const uint shader);
};

struct ShaderProgram {
    static uint create();

    static void attachShader(const uint shaderProgram, const uint shader);

    static void link(const uint shaderProgram);

    static void destroy(const uint shaderProgram);
};
}

#endif