#ifndef ALGINE_IMAGEBUILDER_H
#define ALGINE_IMAGEBUILDER_H

#include <algine/core/Builder.h>

#include <algine/types.h>

namespace algine {
class AL_EXPORT ImageBuilder: public Builder {
public:
    ImageBuilder();

    void setFormat(uint format);
    void setWidth(uint width);
    void setHeight(uint height);

    uint getFormat() const;
    uint getWidth() const;
    uint getHeight() const;

protected:
    uint m_format;
    uint m_width;
    uint m_height;
};
}

#endif //ALGINE_IMAGEBUILDER_H
