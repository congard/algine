#ifndef ALGINE_ICON_H
#define ALGINE_ICON_H

#include <algine/core/Engine.h>
#include <algine/core/Ptr.h>

#include <string>

namespace algine {
class Icon {
public:
    using PixelData = Ptr<unsigned char[]>;

public:
    Icon();
    explicit Icon(const std::string &path, const std::shared_ptr<IOSystem> &ioSystem = nullptr);

    void setWidth(int width);
    void setHeight(int height);
    void setPixels(const PixelData &pixelData);

    int getWidth() const;
    int getHeight() const;
    const PixelData& getPixelData() const;

private:
    int m_width;
    int m_height;
    PixelData m_pixelData;
};
}

#endif //ALGINE_ICON_H
