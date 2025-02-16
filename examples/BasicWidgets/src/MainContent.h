#ifndef ALGINE_EXAMPLES_MAINCONTENT_H
#define ALGINE_EXAMPLES_MAINCONTENT_H

#include <algine/window/unified/UnifiedEventHandler.h>
#include <algine/window/Content.h>

using namespace algine;

namespace UI {
class MainScene;
}

class MainContent: public Content, public UnifiedEventHandler {
public:
    MainContent(Object *parent);

    void onShow() override;
    void render() override;

private:
    UI::MainScene *m_scene;
};

#endif//ALGINE_EXAMPLES_MAINCONTENT_H
