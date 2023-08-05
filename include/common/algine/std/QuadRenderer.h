#ifndef ALGINE_QUADRENDERER_H
#define ALGINE_QUADRENDERER_H

#include <algine/core/InputLayout.h>
#include <algine/core/buffers/ArrayBuffer.h>
#include <algine/core/shader/Shader.h>

namespace algine {
class QuadRenderer: public Object {
public:
    explicit QuadRenderer(uint inPosLocation = 0, Object *parent = defaultParent());

    void draw();

    InputLayout* getInputLayout() const;
    ArrayBuffer* getArrayBuffer() const;

    static Shader* getVertexShader();

private:
    InputLayout *m_inputLayout;
    ArrayBuffer *m_arrayBuffer;
};
}

#endif //ALGINE_QUADRENDERER_H
