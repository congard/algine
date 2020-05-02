#ifndef ALGINE_CUBERENDERER_H
#define ALGINE_CUBERENDERER_H

#include <algine/core/InputLayout.h>
#include <algine/core/ArrayBuffer.h>
#include <tulz/macros.h>

namespace algine {
class CubeRenderer {
public:
    explicit CubeRenderer(uint inPosLocation = 0);
    ~CubeRenderer();

    void draw();

    InputLayout *getInputLayout() const;
    ArrayBuffer *getArrayBuffer() const;

    implementVariadicCreate(CubeRenderer)
    implementVariadicDestroy(CubeRenderer)

public:
    InputLayout *m_inputLayout;
    ArrayBuffer *m_arrayBuffer;
};
}

#endif //ALGINE_CUBERENDERER_H
