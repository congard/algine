#ifndef ALGINE_INPUTLAYOUT_H
#define ALGINE_INPUTLAYOUT_H

#include <algine/gpu/InputAttributeDescription.h>
#include <algine/gpu/buffers/ArrayBuffer.h>
#include <algine/gpu/buffers/IndexBuffer.h>
#include <algine/gpu/context/ContextObject.h>

namespace algine {
AL_CONTEXT_OBJECT(InputLayout) {
    AL_CONTEXT_OBJECT_IMPL(InputLayout)

public:
    explicit InputLayout(Object *parent = defaultParent());
    ~InputLayout() override;

    void bind() const;
    void unbind() const;
    void addAttribute(const InputAttributeDescription &inputAttribDescription, const ArrayBuffer *arrayBuffer) const;
    void setIndexBuffer(const IndexBuffer *indexBuffer) const;
};
}

#endif //ALGINE_INPUTLAYOUT_H
