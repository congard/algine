#ifndef ALGINE_QUADRENDERER_H
#define ALGINE_QUADRENDERER_H

#include <algine/core/InputLayout.h>
#include <algine/core/buffers/ArrayBuffer.h>
#include <tulz/macros.h>

namespace algine {
class QuadRenderer {
public:
    explicit QuadRenderer(uint inPosLocation = 0);
    ~QuadRenderer();

    void draw();

    InputLayout *getInputLayout() const;
    ArrayBuffer *getArrayBuffer() const;

public:
    InputLayout *m_inputLayout;
    ArrayBuffer *m_arrayBuffer;
};
}

#endif //ALGINE_QUADRENDERER_H
