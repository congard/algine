#ifndef ALGINE_IMAGECREATOR_H
#define ALGINE_IMAGECREATOR_H

#include <algine/core/Creator.h>

#include <algine/types.h>

namespace algine {
class ImageCreator: public Creator {
public:
    ImageCreator();

    void setFormat(uint format);
    void setWidth(uint width);
    void setHeight(uint height);

    uint getFormat() const;
    uint getWidth() const;
    uint getHeight() const;

    static void registerLuaUsertype(Lua *lua, sol::global_table *tenv);

protected:
    uint m_format;
    uint m_width;
    uint m_height;
};
}

#endif //ALGINE_IMAGECREATOR_H
