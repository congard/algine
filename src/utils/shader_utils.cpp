/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
 * @author congard
 */

#ifndef ALGINE_SHADER_UTILS_CPP
#define ALGINE_SHADER_UTILS_CPP

#include <GL/glew.h>
#include <iostream>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/* --- shaders utils --- */

namespace su {
    void pointer(int location, int count, GLuint buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glVertexAttribPointer(
            location, // attribute location
            count,    // count (2 or 3)
            GL_FLOAT, // type
            GL_FALSE, // is normalized?
            0,        // step
           (void *)0  // offset
        );
    }

    void setVec3(GLuint location, glm::vec3 vec) {
        glUniform3f(location, vec.x, vec.y, vec.z);
    }

    void setMat4(GLuint location, glm::mat4 mat) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

void getShaderInfoLog(GLint shader, GLchar *buff, int size, int type) {
    GLint success;

    glGetShaderiv(shader, type, &success);

    if (!success) glGetShaderInfoLog(shader, size, NULL, buff);
}

void getShaderInfoLog(GLint shader, int type) {
    GLchar infoLog[512];
    getShaderInfoLog(shader, infoLog, sizeof(infoLog), type);
    #ifdef ALGINE_LOGGING
    std::cout << "=== Info log begin ===\n" << infoLog << "\n=== Info log end ===\n";
    #endif
}

GLint compileShader(const GLchar *shaderSource, GLint type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    // Check for compile time errors
    getShaderInfoLog(shader, GL_COMPILE_STATUS);
    return shader;
}

char *readShader(const char *path) {
    FILE *file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *result = new char[size + 1];
    fseek(file, 0, SEEK_SET);
    fread(result, 1, size, file);
    fclose(file);
    result[size] = '\0';
    return result;
}
 
/* --- programs utils --- */

void getProgramInfoLog(GLint program, GLchar *buff, int size, int type) {
    GLint success;

    glGetProgramiv(program, type, &success);

    if (!success) glGetProgramInfoLog(program, size, NULL, buff);
}

void getProgramInfoLog(GLint program, int type) {
    GLchar infoLog[512];
    getProgramInfoLog(program, infoLog, sizeof(infoLog), type);
    #ifdef ALGINE_LOGGING
    std::cout << "=== Info log begin ===\n" << infoLog << "\n=== Info log end ===\n";
    #endif
}

/**
 * Program with vertex, fragment shaders
 */
GLuint createProgram(GLint vertexShaderId, GLint fragmentShaderId) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShaderId);
    glAttachShader(program, fragmentShaderId);
    return program;
}

/**
 * Program with vertex, fragment, geometry shaders
 */
GLuint createProgram(GLint vertexShaderId, GLint fragmentShaderId, GLint geometryShaderId) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShaderId);
    glAttachShader(program, fragmentShaderId);
    glAttachShader(program, geometryShaderId);
    return program;
}

/* --- classes --- */

#define GLSL_ATTRIBUTE 0
#define GLSL_UNIFORM 1

class ShaderProgram {
    public:
        GLuint programId, vertexShaderId, fragmentShaderId, geometryShaderId;
        std::map<const char*, GLuint> valuesIds;

        void init(const char *vertexShaderPath, const char *fragmentShaderPath, const char *geometryShaderPath) {
            // reading shaders
            char *vertexShaderSource = readShader(vertexShaderPath);
            char *fragmentShaderSource = readShader(fragmentShaderPath);
            char *geometryShaderSource = geometryShaderPath == nullptr ? nullptr : readShader(geometryShaderPath);

            // compiling shaders
            vertexShaderId = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
            fragmentShaderId = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
            geometryShaderId = geometryShaderPath == nullptr ? 0 : compileShader(geometryShaderSource, GL_GEOMETRY_SHADER);

            // making and linking program
            programId = createProgram(vertexShaderId, fragmentShaderId, geometryShaderId);
            glLinkProgram(programId);
            
            // deleting 
            glDeleteShader(vertexShaderId);
            glDeleteShader(fragmentShaderId);
            delete[] vertexShaderSource;
            delete[] fragmentShaderSource;
            if (geometryShaderPath != nullptr) {
                glDeleteShader(geometryShaderId);
                delete[] geometryShaderSource;
            }
        }

        void init(const char *vertexShaderPath, const char *fragmentShaderPath) {
            init(vertexShaderPath, fragmentShaderPath, nullptr);
        }

        void loadValueId(const char *name, GLuint type) {
             if (type == GLSL_ATTRIBUTE) valuesIds[name] = glGetAttribLocation(programId, name);
            else valuesIds[name] = glGetUniformLocation(programId, name);
        }

        GLuint getValueId(const char *name) {
            return valuesIds.find(name)->second;
        }

        ~ShaderProgram() {
            glDeleteProgram(programId);
            #ifdef ALGINE_LOGGING
            std::cout << "Program " << programId << " deleted\n";
            #endif
        }
};

#endif