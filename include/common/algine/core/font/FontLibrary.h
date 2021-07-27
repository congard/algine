#ifndef ALGINE_FONTLIBRARY_H
#define ALGINE_FONTLIBRARY_H

#include <algine/core/font/Font.h>

#include <map>

namespace algine {
class FontLibrary {
public:
    static void add(const Font &font);
    static Font get(const std::string &name, Font::Style style);
    static bool exists(const std::string &name, Font::Style style);
    static void remove(const std::string &name, Font::Style style);
    static void remove(const Font &font);

    static void optimize();
    static uint size();

private:
    static std::map<unsigned long, Font> m_library;
};
}

#endif //ALGINE_FONTLIBRARY_H
