#include <algine/core/font/Font.h>
#include <algine/core/font/FontLibrary.h>
#include <algine/core/Engine.h>

#include <freetype/ftsnames.h>
#include <freetype/ttnameid.h>

#include <tulz/Path.h>

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace tulz;

namespace algine {
struct FontSearchInfo {
    bool found = false;
    Font font;
};

FontSearchInfo findFont(const std::forward_list<Path> &children, const std::string &name, Font::Style style) {
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

            if (font.isValid() && (font.getName() == name) && (font.getStyle() == style || style == Font::Style::Any)) {
                return {true, font};
            } else if (font.isValid() && font.use_count() == 2) {
                // removes font from FontLibrary if it has not been previously added
                // 2 because font owned by this and by FontLibrary; this check is
                // performed just in case, findFont shouldn't be called if font with
                // the same name and style has been already loaded
                FontLibrary::remove(font);
            }
        }
    }

    return {false};
}

inline FontSearchInfo findFont(const std::string &name, Font::Style style) {
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

Font::Font() noexcept = default;

Font::Font(const std::string &name, Style style) {
    load(name, style);
}

Font::Font(const std::string &path) {
    loadPath(path);
}

void Font::load(const std::string &name, Style style) {
    auto fromLibrary = [&](Style style) {
        if (FontLibrary::exists(name, style)) {
            *this = FontLibrary::get(name, style);
            return true;
        }

        return false;
    };

    auto fromSystem = [&]() {
        auto info = findFont(name, style);

        if (info.found) {
            std::swap(*this, info.font);
        }
    };

    if (style == Style::Any) {
        if (fromLibrary(Style::Regular) || fromLibrary(Style::Bold) || fromLibrary(Style::Italic)) {
            return;
        }

        fromSystem();
    } else if (!fromLibrary(style)) {
        fromSystem();
    }
}

struct LoadedFont {
    FT_Byte *buffer {nullptr};
    long bufferSize {};
    FT_Face face {};
};

void Font::loadPath(const std::string &path, const std::shared_ptr<IOSystem> &inIo) {
    std::shared_ptr<IOSystem> io = inIo ? inIo : Engine::getDefaultIOSystem();

    auto stream = io->open(path, IOStream::Mode::Read);
    long size = static_cast<long>(stream->size());
    auto buffer = new FT_Byte[size];
    stream->read(buffer, size, 1);
    stream->close();

    FT_Face face;

    auto error = FT_New_Memory_Face(
            reinterpret_cast<FT_Library>(Engine::m_fontLibrary),
            buffer, size, 0, &face);

    if (!error) {
        m_face.reset(new LoadedFont {buffer, size, face}, [](LoadedFont *font) {
            if (Engine::m_fontLibrary != nullptr && font != nullptr && font->face != nullptr) {
                FT_Done_Face(font->face);
            }

            delete[] font->buffer;
        });

        if (FontLibrary::exists(getName(), getStyle())) {
            *this = FontLibrary::get(getName(), getStyle());
        } else {
            FontLibrary::add(*this);
        }
    } else {
        delete[] buffer;
    }
}

inline std::string getProperty(FT_UShort property, const std::shared_ptr<void> &ftFace) {
    auto face = static_cast<LoadedFont*>(ftFace.get())->face;

    for (int i = 0; i < FT_Get_Sfnt_Name_Count(face); i++) {
        FT_SfntName_ properties {};

        FT_Get_Sfnt_Name(face, i, &properties);

        if (properties.name_id == property) {
            return {reinterpret_cast<const char *>(properties.string), properties.string_len};
        }
    }

    return {};
}

Font::Style Font::getStyle() const {
    std::string prop = getProperty(TT_NAME_ID_FONT_SUBFAMILY, m_face);

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

std::string Font::getCopyright() const {
    return getProperty(TT_NAME_ID_COPYRIGHT, m_face);
}

std::string Font::getFontFamily() const {
    return getProperty(TT_NAME_ID_FONT_FAMILY, m_face);
}

std::string Font::getUniqueId() const {
    return getProperty(TT_NAME_ID_UNIQUE_ID, m_face);
}

std::string Font::getName() const {
    return getProperty(TT_NAME_ID_FULL_NAME, m_face);
}

std::string Font::getPSName() const {
    return getProperty(TT_NAME_ID_PS_NAME, m_face);
}

std::string Font::getVersion() const {
    return getProperty(TT_NAME_ID_VERSION_STRING, m_face);
}

std::string Font::getTrademark() const {
    return getProperty(TT_NAME_ID_TRADEMARK, m_face);
}

std::string Font::getManufacturer() const {
    return getProperty(TT_NAME_ID_MANUFACTURER, m_face);
}

std::string Font::getDesigner() const {
    return getProperty(TT_NAME_ID_DESIGNER, m_face);
}

std::string Font::getDescription() const {
    return getProperty(TT_NAME_ID_DESCRIPTION, m_face);
}

std::string Font::getVendorUrl() const {
    return getProperty(TT_NAME_ID_VENDOR_URL, m_face);
}

std::string Font::getDesignerUrl() const {
    return getProperty(TT_NAME_ID_DESIGNER_URL, m_face);
}

std::string Font::getLicense() const {
    return getProperty(TT_NAME_ID_LICENSE, m_face);
}

std::string Font::getLicenseUrl() const {
    return getProperty(TT_NAME_ID_LICENSE_URL, m_face);
}

std::string Font::getTypographicFamily() const {
    return getProperty(TT_NAME_ID_TYPOGRAPHIC_FAMILY, m_face);
}

std::string Font::getTypographicSubFamily() const {
    return getProperty(TT_NAME_ID_TYPOGRAPHIC_SUBFAMILY, m_face);
}

bool Font::isValid() const {
    return m_face != nullptr;
}

void* Font::native_handle() const {
    return static_cast<LoadedFont*>(m_face.get())->face;
}

long Font::use_count() const {
    return m_face.use_count();
}
}
