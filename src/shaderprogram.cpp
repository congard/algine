#ifndef ALGINE_SHADERPROGRAM_CPP
#define ALGINE_SHADERPROGRAM_CPP

#include <iostream>
#include <GL/glew.h>
#include "texture.cpp"

namespace algine {
#define useShaderProgram(shaderprogram) glUseProgram(shaderprogram)

#define ALGINE_VERTEX_SHADER GL_VERTEX_SHADER
#define ALGINE_FRAGMENT_SHADER GL_FRAGMENT_SHADER
#define ALGINE_GEOMETRY_SHADER GL_GEOMETRY_SHADER

void getShaderInfoLog(GLint shader, int type) {
    GLint infoLogSize, success;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);
    
    if (infoLogSize == 0) return;

    GLchar infoLog[infoLogSize];
    glGetShaderiv(shader, type, &success);

    if (!success) {
        glGetShaderInfoLog(shader, infoLogSize, nullptr, infoLog);
        std::cout << "Shader info log (for shader id " << shader << "): " << infoLog << "\n";
    }
}

void getProgramInfoLog(GLint program, int type) {
    GLint infoLogSize, success;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
    
    if (infoLogSize == 0) return;

    GLchar infoLog[infoLogSize];
    glGetProgramiv(program, type, &success);

    if (!success) {
        glGetProgramInfoLog(program, infoLogSize, nullptr, infoLog);
        std::cout << "Program info log (for program id " << program << "): " << infoLog << "\n";
    }
}

struct Shader {
    static GLuint create(const GLuint type) {
        return glCreateShader(type);
    }

    static void compile(const GLuint shader, const std::string &source) {
        const char *c_str = source.c_str();
        glShaderSource(shader, 1, &c_str, nullptr);
        glCompileShader(shader);
        getShaderInfoLog((GLint) shader, GL_COMPILE_STATUS);
    }

    static void destroy(const GLuint shader) {
        glDeleteShader(shader);
    }
};

struct ShaderProgram {
    static GLuint create() {
        return glCreateProgram();
    }

    static void attachShader(const GLuint shaderProgram, const GLuint shader) {
        glAttachShader(shaderProgram, shader);
    }

    static void link(const GLuint shaderProgram) {
        glLinkProgram(shaderProgram);
        getProgramInfoLog((GLint) shaderProgram, GL_LINK_STATUS);
    }

    static void destroy(const GLuint shaderProgram) {
        glDeleteProgram(shaderProgram);
    }
};
}

#endif /* ALGINE_SHADERPROGRAM_CPP */
