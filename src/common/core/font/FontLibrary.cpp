#include <algine/core/font/FontLibrary.h>

#include "../djb2.h"

namespace algine {
std::map<unsigned long, Font> FontLibrary::m_library;

inline auto getHash(const std::string &name, Font::Style style) {
    std::string styleStr = std::to_string(static_cast<int>(style));
    return hash_djb2((unsigned char *) (name + " " + styleStr).c_str());
}

void FontLibrary::add(const Font &font) {
    auto hash = getHash(font.getName(), font.getStyle());
    m_library[hash] = font;
}

Font FontLibrary::get(const std::string &name, Font::Style style) {
    auto hash = getHash(name, style);
    return m_library[hash];
}

bool FontLibrary::exists(const std::string &name, Font::Style style) {
    auto hash = getHash(name, style);
    return m_library.find(hash) != m_library.end();
}

void FontLibrary::remove(const std::string &name, Font::Style style) {
    auto hash = getHash(name, style);
    m_library.erase(hash);
}

void FontLibrary::remove(const Font &font) {
    remove(font.getName(), font.getStyle());
}

void FontLibrary::optimize() {
    for (auto it = m_library.begin(); it != m_library.end();) {
        if (it->second.use_count() == 1) { // owned only by FontLibrary
            it = m_library.erase(it);
        } else {
            ++it;
        }
    }
}

uint FontLibrary::size() {
    return m_library.size();
}
}
