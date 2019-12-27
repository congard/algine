#include <algine/debug.h>
#include <GL/glew.h>

#include <fstream>
#include <iostream>

// taken from https://vallentin.dev/2015/02/23/debugging-opengl
void debugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
             const void *userParam) {
#ifdef ALGINE_DEBUG_TO_FILE
#   ifndef ALGINE_DEBUG_FILE_NAME
#       define ALGINE_DEBUG_FILE_NAME "debug.txt"
#   endif

    std::ofstream out;
    out.open(ALGINE_DEBUG_FILE_NAME, std::ios::app);
#   define log out
#else
#   define log std::cout
#endif

    // Some debug messages are just annoying informational messages
    switch (id) {
        case 131185: // glBufferData
            return;
    }

    log << "Message: " << message << "\n";
    log << "Source: ";

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            log << "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            log << "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            log << "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            log << "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            log << "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            log << "Other";
            break;
        default:
            log << "Unknown source " << source << "\n";
            break;
    }

    log << "\n";
    log << "Type: ";

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            log << "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            log << "Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            log << "Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            log << "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            log << "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            log << "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            log << "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            log << "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            log << "Other";
            break;
        default:
            log << "Unknown type " << type << "\n";
            break;
    }

    log << "\n";
    log << "ID: " << id << "\n";
    log << "Severity: ";

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            log << "High";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            log << "Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            log << "Low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            log << "Notification";
            break;
        default:
            log << "Unknown severity " << severity << "\n";
            break;
    }

    log << "\n\n";

#ifdef ALGINE_DEBUG_TO_FILE
    out.close();
#endif
}

namespace algine {
void printGLError(const char *const message) {
    int err = glGetError();
    if (err != 0)
        std::cout << "Message: " << message << "\nError: " << err << "\n\n"; \
}

void enableGLDebugOutput() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugMessage, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}
}