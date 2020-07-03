#ifndef ALGINE_SOPCONSTANTS_H
#define ALGINE_SOPCONSTANTS_H

#define constant(name, val) constexpr char name[] = val;

namespace SOPConstants {
enum {
    FramebufferObject,
    RenderbufferObject,
    Texture2DObject,
    TextureCubeObject,
    ArrayBufferObject,
    IndexBufferObject,
    ShaderProgramObject
};

constant(FramebufferStr, "Framebuffer")
constant(RenderbufferStr, "Renderbuffer")
constant(Texture2DStr, "Texture2D")
constant(TextureCubeStr, "TextureCube")
constant(ArrayBufferStr, "ArrayBuffer")
constant(IndexBufferStr, "IndexBuffer")
constant(ShaderProgramStr, "ShaderProgram")
}

#undef constant

#endif //ALGINE_SOPCONSTANTS_H
