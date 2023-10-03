#include "MainScene.h"
#include "MainLayer.h"

namespace UI {
MainScene::MainScene(Object *parent)
    : Widgets::Scene(parent)
{
    auto layer = new MainLayer(this);
    layer->show();
}
}
