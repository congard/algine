#ifndef ALGINE_QUADRENDERER_H
#define ALGINE_QUADRENDERER_H

#include <algine/InputLayout.h>
#include <algine/ArrayBuffer.h>
#include <tulz/macros.h>

namespace algine {
class QuadRenderer {
public:
    // TODO: remove inTexCoordLocation. It must be calculated automatically
    //  add getter for m_arrayBuffer (after removal m_vertexArrayBuffer and m_texCoordArrayBuffer)
    QuadRenderer(int inPosLocation, int inTexCoordLocation);
    ~QuadRenderer();

    void draw();

    InputLayout *getInputLayout() const;

    // implementVariadicCreate(QuadRenderer) // TODO: add after removal inTexCoordLocation
    implementVariadicDestroy(QuadRenderer)

public:
    InputLayout *m_inputLayout;
    ArrayBuffer *m_vertexArrayBuffer, *m_texCoordArrayBuffer;
};
}

#endif //ALGINE_QUADRENDERER_H
