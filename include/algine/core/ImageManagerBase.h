#ifndef ALGINE_IMAGEMANAGERBASE_H
#define ALGINE_IMAGEMANAGERBASE_H

#include <algine/core/ManagerBase.h>

namespace algine {
class ImageManagerBase: public ManagerBase {
public:
    ImageManagerBase();

    void setFormat(uint format);
    void setWidth(uint width);
    void setHeight(uint height);

    uint getFormat() const;
    uint getWidth() const;
    uint getHeight() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

protected:
    uint m_format;
    uint m_width, m_height;
};
}

#endif //ALGINE_IMAGEMANAGERBASE_H
