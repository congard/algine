#ifndef ALGINE_INPUTLAYOUT_H
#define ALGINE_INPUTLAYOUT_H

#include <algine/core/buffers/ArrayBuffer.h>
#include <algine/core/buffers/IndexBuffer.h>
#include <algine/core/InputAttributeDescription.h>
#include <algine/templates.h>
#include <tulz/macros.h>

namespace algine {
class InputLayout {
    friend class Engine;

public:
    InputLayout();
    ~InputLayout();

    void bind() const;
    void unbind() const;
    void addAttribute(const InputAttributeDescription &inputAttribDescription, const ArrayBuffer *arrayBuffer) const;
    void setIndexBuffer(const IndexBuffer *indexBuffer) const;

    implementVariadicCreate(InputLayout)
    implementVariadicDestroy(InputLayout)

private:
    uint m_id;
};
}

#endif //ALGINE_INPUTLAYOUT_H
