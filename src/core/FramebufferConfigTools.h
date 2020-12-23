#ifndef ALGINE_FRAMEBUFFERCONFIGTOOLS_H
#define ALGINE_FRAMEBUFFERCONFIGTOOLS_H

#include <algine/core/Framebuffer.h>

#include <stdexcept>

#include "internal/ConfigStrings.h"

namespace algine::Config {
inline uint stringToAttachment(const string &str) {
    // if str starts with Color
    if (str.rfind(Color) == 0) {
        uint colorLength = strlen(Color);

        // catch 'color' case
        if (str.size() == colorLength)
            throw runtime_error("Can't parse color attachment: index does not specified");

        // catch 'color1x' cases
        char *endPtr;
        uint index = strtol(str.data() + colorLength, &endPtr, 10);

        if (*endPtr != '\0')
            throw runtime_error("Broken color attachment: '" + str + "'");

        return Framebuffer::ColorAttachmentZero + index;
    }

#define attachment_string(name, value) if (str == name) return Framebuffer::value

    attachment_string(Depth, DepthAttachment);
    attachment_string(Stencil, StencilAttachment);
    attachment_string(DepthStencil, DepthStencilAttachment);
    attachment_string(Empty, EmptyAttachment);

#undef attachment_string

    throw runtime_error("Unknown attachment '" + str + "'");
}

inline string attachmentToString(uint attachment) {
    // color attachment
    if (attachment >= Framebuffer::ColorAttachmentZero && attachment < Framebuffer::DepthAttachment)
        return Color + to_string(attachment - Framebuffer::ColorAttachmentZero);

    switch (attachment) {
        case Framebuffer::DepthAttachment: return Depth;
        case Framebuffer::StencilAttachment: return Stencil;
        case Framebuffer::DepthStencilAttachment: return DepthStencil;
        case Framebuffer::EmptyAttachment: return Empty;
        default: break;
    }

    throw runtime_error("Unknown attachment " + to_string(attachment));
}
}

#endif //ALGINE_FRAMEBUFFERCONFIGTOOLS_H
