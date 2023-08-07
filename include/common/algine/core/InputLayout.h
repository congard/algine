#ifndef ALGINE_INPUTLAYOUT_H
#define ALGINE_INPUTLAYOUT_H

#include <algine/core/buffers/ArrayBuffer.h>
#include <algine/core/buffers/IndexBuffer.h>
#include <algine/core/InputAttributeDescription.h>
#include <algine/core/context/ContextObject.h>

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
