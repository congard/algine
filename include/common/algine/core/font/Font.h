#ifndef ALGINE_FONT_H
#define ALGINE_FONT_H

#include <algine/core/io/IOSystem.h>

#include <string>
#include <memory>

namespace algine {
class Font {
public:
    enum class Style {
        Regular,
        Bold,
        Italic,
        Any,
        Unknown
    };

public:
    Font();
    Font(const std::string &name, Style style);
    explicit Font(const std::string &path);

    void load(const std::string &name, Style style);
    void loadPath(const std::string &path, const std::shared_ptr<IOSystem> &io = nullptr);

    Style getStyle() const;

    std::string getCopyright() const;
    std::string getFontFamily() const;
    std::string getUniqueId() const;
    std::string getName() const;
    std::string getPSName() const;
    std::string getVersion() const;
    std::string getTrademark() const;
    std::string getManufacturer() const;
    std::string getDesigner() const;
    std::string getDescription() const;
    std::string getVendorUrl() const;
    std::string getDesignerUrl() const;
    std::string getLicense() const;
    std::string getLicenseUrl() const;
    std::string getTypographicFamily() const;
    std::string getTypographicSubFamily() const;

    bool isLoaded() const;

    void* native_handle() const;

private:
    std::shared_ptr<void> m_face;
};
}

#endif //ALGINE_FONT_H
