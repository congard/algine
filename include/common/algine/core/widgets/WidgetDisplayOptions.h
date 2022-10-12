#ifndef ALGINE_WIDGETDISPLAYOPTIONS_H
#define ALGINE_WIDGETDISPLAYOPTIONS_H

namespace algine {
class Framebuffer;
class Painter;

struct WidgetDisplayOptions {
    int width {0};
    int height {0};
    int paddingLeft {0};
    int paddingTop {0};
    int paddingRight {0};
    int paddingBottom {0};
    Framebuffer *framebuffer {nullptr};
    Painter *painter {nullptr};
};
}

#endif //ALGINE_WIDGETDISPLAYOPTIONS_H
