#ifndef ALGINE_FONTLIBRARY_H
#define ALGINE_FONTLIBRARY_H

#include <algine/core/font/Font.h>
#include <algine/types.h>

#include <map>

namespace algine {
class FontLibrary {
public:
    static void add(const Font &font);
    static Font get(std::string_view name, Font::Style style);
    static bool exists(std::string_view name, Font::Style style);
    static void remove(std::string_view name, Font::Style style);
    static void remove(const Font &font);

    static void optimize();
    static uint size();

    static void setDefault(const Font &font);
    static const Font& getDefault();

private:
    static std::map<unsigned long, Font> m_library;
    static Font m_default;
};
}

#endif //ALGINE_FONTLIBRARY_H
