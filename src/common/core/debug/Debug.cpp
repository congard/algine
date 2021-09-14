#include "Debug.h"

#include <algine/core/Engine.h>

#include <algine/gl.h>

namespace algine {
// taken from https://vallentin.dev/2015/02/23/debugging-opengl
void debugMessagesHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
                          const void *userParam)
{
    auto writer = static_cast<DebugWriter*>(const_cast<void*>(userParam));
    auto logger = writer->logger();

    // Some debug messages are just annoying informational messages
    if (id == 131185) // glBufferData
        return;

    logger << "Message: " << message << "\n";
    logger << "Source: ";

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            logger << "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            logger << "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            logger << "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            logger << "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            logger << "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            logger << "Other";
            break;
        default:
            logger << "Unknown source " << source << "\n";
            break;
    }

    logger << "\n";
    logger << "Type: ";

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            logger << "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            logger << "Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            logger << "Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            logger << "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            logger << "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            logger << "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            logger << "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            logger << "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            logger << "Other";
            break;
        default:
            logger << "Unknown type " << type << "\n";
            break;
    }

    logger << "\n";
    logger << "ID: " << id << "\n";
    logger << "Severity: ";

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            logger << "High";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            logger << "Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            logger << "Low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            logger << "Notification";
            break;
        default:
            logger << "Unknown severity " << severity << "\n";
            break;
    }

    logger << "\n";
}

void enableDebugOutput() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugMessagesHandler, Engine::getDebugWriter().get());
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}
}
