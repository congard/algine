#ifndef ALGINE_QUADRENDERER_H
#define ALGINE_QUADRENDERER_H

#include <algine/InputLayout.h>
#include <algine/ArrayBuffer.h>
#include <tulz/macros.h>

namespace algine {
class QuadRenderer {
public:
    explicit QuadRenderer(uint inPosLocation = 0);
    ~QuadRenderer();

    void draw();

    InputLayout *getInputLayout() const;
    ArrayBuffer *getArrayBuffer() const;

    implementVariadicCreate(QuadRenderer)
    implementVariadicDestroy(QuadRenderer)

public:
    InputLayout *m_inputLayout;
    ArrayBuffer *m_arrayBuffer;
};
}

#endif //ALGINE_QUADRENDERER_H
