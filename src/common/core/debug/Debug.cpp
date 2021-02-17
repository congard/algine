#include "Debug.h"

#include <algine/core/Engine.h>

#include <algine/gl.h>

namespace algine {
// taken from https://vallentin.dev/2015/02/23/debugging-opengl
void debugMessagesHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
                          const void *userParam)
{
    auto writer = static_cast<DebugWriter*>(const_cast<void*>(userParam));
    writer->begin();

    auto &stream = writer->stream();

    // Some debug messages are just annoying informational messages
    if (id == 131185) // glBufferData
        return;

    stream << "Message: " << message << "\n";
    stream << "Source: ";

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            stream << "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            stream << "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            stream << "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            stream << "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            stream << "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            stream << "Other";
            break;
        default:
            stream << "Unknown source " << source << "\n";
            break;
    }

    stream << "\n";
    stream << "Type: ";

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            stream << "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            stream << "Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            stream << "Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            stream << "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            stream << "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            stream << "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            stream << "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            stream << "Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            stream << "Other";
            break;
        default:
            stream << "Unknown type " << type << "\n";
            break;
    }

    stream << "\n";
    stream << "ID: " << id << "\n";
    stream << "Severity: ";

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            stream << "High";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            stream << "Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            stream << "Low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            stream << "Notification";
            break;
        default:
            stream << "Unknown severity " << severity << "\n";
            break;
    }

    stream << "\n";

    writer->end();
}

void enableDebugOutput() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugMessagesHandler, Engine::getDebugWriter().get());
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}
}
