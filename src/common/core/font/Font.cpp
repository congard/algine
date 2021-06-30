#include <algine/core/font/Font.h>
#include <algine/core/font/FontEngine.h>

#include <freetype/ftsnames.h>
#include <freetype/ttnameid.h>

#include <tulz/Path.h>

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace std;
using namespace tulz;

namespace algine {
struct FontSearchInfo {
    bool found = false;
    Font font;
};

FontSearchInfo findFont(const std::forward_list<Path> &children, const string &name, Font::Style style) {
    for (const auto &child : children) {
        if (child.isDirectory()) {
            auto dirChildren = child.listChildren();

            for (auto &dirChild : dirChildren) {
                dirChild = Path::join(child, dirChild);
            }

            auto info = findFont(dirChildren, name, style);

            if (info.found) {
                return info;
            }
        } else {
            Font font(child.toString());

            if (font.getName() == name && (font.getStyle() == style || style == Font::Style::Any)) {
                return {true, font};
            }
        }
    }

    return {false};
}

inline FontSearchInfo findFont(const string &name, Font::Style style) {
#ifdef __ANDROID__
    return findFont({"/system/fonts"_p}, name, style);
#elif defined(__linux__)
    return findFont({"/usr/share/fonts/truetype"_p, "/usr/share/fonts/opentype"_p}, name, style);
#elif defined(_WIN32)
    TCHAR winDir[MAX_PATH];
    GetWindowsDirectory(winDir, MAX_PATH);
    return findFont({Path(Path::join(string(winDir), "Fonts"))}, name, style);
#endif
}

Font::Font() = default;

Font::Font(const string &name, Style style) {
    load(name, style);
}

Font::Font(const string &path) {
    loadPath(path);
}

void Font::load(const string &name, Style style) {
    auto info = findFont(name, style);

    if (info.found) {
        std::swap(*this, info.font);
    }
}

void Font::loadPath(const string &path) {
    FT_Face face;

    auto error = FT_New_Face(reinterpret_cast<FT_Library>(FontEngine::m_fontLibrary), path.c_str(), 0, &face);

    if (!error) {
        m_face.reset(face, [&](FT_Face face) {
            if (isLoaded()) {
                FT_Done_Face(face);
            }
        });
    }
}

inline string getProperty(FT_UShort property, const shared_ptr<void> &ftFace) {
    auto face = static_cast<FT_Face>(ftFace.get());

    for (int i = 0; i < FT_Get_Sfnt_Name_Count(face); i++) {
        FT_SfntName_ properties {};

        FT_Get_Sfnt_Name(face, i, &properties);

        if (properties.name_id == property) {
            return string(reinterpret_cast<const char *>(properties.string), properties.string_len);
        }
    }

    return string();
}

Font::Style Font::getStyle() const {
    string prop = getProperty(TT_NAME_ID_FONT_SUBFAMILY, m_face);

    if (prop == "Regular") {
        return Style::Regular;
    } else if (prop == "Bold") {
        return Style::Bold;
    } else if (prop == "Italic") {
        return Style::Italic;
    } else {
        return Style::Unknown;
    }
}

string Font::getCopyright() const {
    return getProperty(TT_NAME_ID_COPYRIGHT, m_face);
}

string Font::getFontFamily() const {
    return getProperty(TT_NAME_ID_FONT_FAMILY, m_face);
}

string Font::getUniqueId() const {
    return getProperty(TT_NAME_ID_UNIQUE_ID, m_face);
}

string Font::getName() const {
    return getProperty(TT_NAME_ID_FULL_NAME, m_face);
}

string Font::getPSName() const {
    return getProperty(TT_NAME_ID_PS_NAME, m_face);
}

string Font::getVersion() const {
    return getProperty(TT_NAME_ID_VERSION_STRING, m_face);
}

string Font::getTrademark() const {
    return getProperty(TT_NAME_ID_TRADEMARK, m_face);
}

string Font::getManufacturer() const {
    return getProperty(TT_NAME_ID_MANUFACTURER, m_face);
}

string Font::getDesigner() const {
    return getProperty(TT_NAME_ID_DESIGNER, m_face);
}

string Font::getDescription() const {
    return getProperty(TT_NAME_ID_DESCRIPTION, m_face);
}

string Font::getVendorUrl() const {
    return getProperty(TT_NAME_ID_VENDOR_URL, m_face);
}

string Font::getDesignerUrl() const {
    return getProperty(TT_NAME_ID_DESIGNER_URL, m_face);
}

string Font::getLicense() const {
    return getProperty(TT_NAME_ID_LICENSE, m_face);
}

string Font::getLicenseUrl() const {
    return getProperty(TT_NAME_ID_LICENSE_URL, m_face);
}

string Font::getTypographicFamily() const {
    return getProperty(TT_NAME_ID_TYPOGRAPHIC_FAMILY, m_face);
}

string Font::getTypographicSubFamily() const {
    return getProperty(TT_NAME_ID_TYPOGRAPHIC_SUBFAMILY, m_face);
}

bool Font::isLoaded() const {
    return m_face != nullptr;
}

const shared_ptr<void>& Font::native_handle() const {
    return m_face;
}
}
