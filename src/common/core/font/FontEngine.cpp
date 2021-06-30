#include <algine/core/font/FontEngine.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace algine {
void* FontEngine::m_fontLibrary;

void FontEngine::init() {
    FT_Init_FreeType(reinterpret_cast<FT_Library *>(&m_fontLibrary));
}

void FontEngine::destroy() {
    FT_Done_FreeType(static_cast<FT_Library>(m_fontLibrary));
}
}
