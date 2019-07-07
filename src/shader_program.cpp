#include <algine/shader_program.h>
#include <GL/glew.h>
#include <algine/texture.h>

namespace algine {

void getShaderInfoLog(int shader, int type) {
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

void getProgramInfoLog(int program, int type) {
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

// Shader
uint Shader::create(const uint type) {
    return glCreateShader(type);
}

void Shader::compile(const uint shader, const std::string &source) {
    const char *c_str = source.c_str();
    glShaderSource(shader, 1, &c_str, nullptr);
    glCompileShader(shader);
    getShaderInfoLog((GLint) shader, GL_COMPILE_STATUS);
}

void Shader::destroy(const uint shader) {
    glDeleteShader(shader);
}

// ShaderProgram
GLuint ShaderProgram::create() {
    return glCreateProgram();
}

void ShaderProgram::attachShader(const uint shaderProgram, const uint shader) {
    glAttachShader(shaderProgram, shader);
}

void ShaderProgram::link(const uint shaderProgram) {
    glLinkProgram(shaderProgram);
    getProgramInfoLog((GLint) shaderProgram, GL_LINK_STATUS);
}

void ShaderProgram::destroy(const uint shaderProgram) {
    glDeleteProgram(shaderProgram);
}

}