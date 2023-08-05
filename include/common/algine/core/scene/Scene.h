#ifndef ALGINE_SCENE_H
#define ALGINE_SCENE_H

#include <algine/core/Object.h>

namespace algine {
class Scene: public Object {
public:
    explicit Scene(Object *parent = nullptr);

    virtual void render();

    using Object::addChild;
    using Object::removeChild;
    using Object::removeChildren;
};
} // algine

#endif //ALGINE_SCENE_H
