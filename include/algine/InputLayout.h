#ifndef ALGINE_INPUTLAYOUT_H
#define ALGINE_INPUTLAYOUT_H

#include <algine/ArrayBuffer.h>
#include <algine/IndexBuffer.h>
#include <algine/InputAttributeDescription.h>
#include <algine/templates.h>

namespace algine {
class InputLayout {
public:
    InputLayout();
    ~InputLayout();

    void bind() const;
    void unbind() const;
    void addAttribute(const InputAttributeDescription &inputAttributeDescription, const ArrayBuffer *arrayBuffer) const;
    void setIndexBuffer(const IndexBuffer *indexBuffer) const;

    implementVariadicCreate(InputLayout)
    implementVariadicDestroy(InputLayout)

public:
    uint m_id;
};
}

#endif //ALGINE_INPUTLAYOUT_H
