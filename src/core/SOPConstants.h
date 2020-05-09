#ifndef ALGINE_SOPCONSTANTS_H
#define ALGINE_SOPCONSTANTS_H

#define constant(name, val) constexpr char name[] = val;

namespace SOPConstants {
enum {
    FramebufferObject,
    RenderbufferObject,
    Texture2DObject,
    TextureCubeObject
};

constant(FramebufferStr, "Framebuffer")
constant(RenderbufferStr, "Renderbuffer")
constant(Texture2DStr, "Texture2D")
constant(TextureCubeStr, "TextureCube")
}

#undef constant

#endif //ALGINE_SOPCONSTANTS_H
