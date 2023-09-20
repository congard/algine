#ifndef ALGINE_CUBERENDERER_H
#define ALGINE_CUBERENDERER_H

#include <algine/core/InputLayout.h>
#include <algine/core/buffers/ArrayBuffer.h>
#include <tulz/macros.h>
#include <memory>

namespace algine {
class AL_EXPORT CubeRenderer {
public:
    explicit CubeRenderer(uint inPosLocation = 0);

    void draw();

    const std::unique_ptr<InputLayout>& getInputLayout() const;
    const std::unique_ptr<ArrayBuffer>& getArrayBuffer() const;

public:
    std::unique_ptr<InputLayout> m_inputLayout;
    std::unique_ptr<ArrayBuffer> m_arrayBuffer;
};
}

#endif //ALGINE_CUBERENDERER_H
