#include <algine/core/buffers/Buffer.h>

#include <algine/core/Engine.h>

#include <GL/glew.h>

#include <string>
#include <cassert>

#include "../SOP.h"

using namespace algine;
using namespace std;

#ifdef ALGINE_SECURE_OPERATIONS
#include "../SOPConstants.h"
#define SOP_BOUND_PTR getBoundBuffer(m_target)
#define SOP_OBJECT_TYPE getBufferObject(m_target)
#define SOP_OBJECT_ID m_id
#define SOP_OBJECT_NAME getBufferObjectName(m_target)

inline void* getBoundBuffer(const uint target) {
    switch (target) {
        case GL_ARRAY_BUFFER:
            return Engine::getBoundArrayBuffer();
        case GL_ELEMENT_ARRAY_BUFFER:
            return Engine::getBoundIndexBuffer();
        case GL_UNIFORM_BUFFER:
            return Engine::getBoundUniformBuffer();
        default:
            assert(0);
    }
}

inline uint getBufferObject(const uint target) {
    switch (target) {
        case GL_ARRAY_BUFFER:
            return SOPConstants::ArrayBufferObject;
        case GL_ELEMENT_ARRAY_BUFFER:
            return SOPConstants::IndexBufferObject;
        case GL_UNIFORM_BUFFER:
            return SOPConstants::UniformBufferObject;
        default:
            assert(0);
    }
}

inline string getBufferObjectName(const uint target) {
    switch (target) {
        case GL_ARRAY_BUFFER:
            return SOPConstants::ArrayBufferStr;
        case GL_ELEMENT_ARRAY_BUFFER:
            return SOPConstants::IndexBufferStr;
        case GL_UNIFORM_BUFFER:
            return SOPConstants::UniformBufferStr;
        default:
            assert(0);
    }
}
#endif

namespace algine {
Buffer::Buffer() {
    glGenBuffers(1, &m_id);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &m_id);
}

void Buffer::bind() const {
    commitBinding()
    glBindBuffer(m_target, m_id);
}

void Buffer::unbind() const {
    checkBinding()
    commitUnbinding()
    glBindBuffer(m_target, 0);
}

void Buffer::setData(const uint size, const void *data, const uint usage) {
    checkBinding()
    glBufferData(m_target, size, data, usage);
}

uint Buffer::getId() const {
    return m_id;
}

Buffer::Buffer(const uint target): Buffer() {
    this->m_target = target;
}
}