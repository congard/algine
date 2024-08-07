#ifndef ALGINE_GLOBALSCENE_H
#define ALGINE_GLOBALSCENE_H

#include <algine/core/scene/Scene.h>
#include <tulz/util/static_initializer.h>

namespace algine {
class AL_EXPORT GlobalScene: public Scene {
    STATIC_INITIALIZER_DECL

public:
    static GlobalScene* getInstance();

protected:
    explicit GlobalScene();

private:
    void setParent(Object *parent) override;

private:
    static GlobalScene *m_instance;
};
}

#endif //ALGINE_GLOBALSCENE_H
