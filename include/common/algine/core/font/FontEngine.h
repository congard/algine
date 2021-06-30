#ifndef ALGINE_FONTENGINE_H
#define ALGINE_FONTENGINE_H

// TODO: tmp class, merge with algine::Engine

namespace algine {
class FontEngine {
    friend class Font;

public:
    static void init();

    static void destroy();

private:
    static void *m_fontLibrary;
};
}

#endif //ALGINE_FONTENGINE_H
