#ifndef ALGINE_QUADRENDERER_H
#define ALGINE_QUADRENDERER_H

#include <algine/core/InputLayout.h>
#include <algine/core/buffers/ArrayBuffer.h>
#include <algine/core/shader/ShaderPtr.h>

namespace algine {
class QuadRenderer {
public:
    explicit QuadRenderer(uint inPosLocation = 0);

    void draw();

    const std::unique_ptr<InputLayout>& getInputLayout() const;
    const std::unique_ptr<ArrayBuffer>& getArrayBuffer() const;

    static ShaderPtr getVertexShader();

public:
    std::unique_ptr<InputLayout> m_inputLayout;
    std::unique_ptr<ArrayBuffer> m_arrayBuffer;
};
}

#endif //ALGINE_QUADRENDERER_H
