#ifndef ALGINE_EXAMPLES_MAINLAYER_H
#define ALGINE_EXAMPLES_MAINLAYER_H

#include <algine/core/widgets/Layer.h>

using namespace algine;

namespace UI {
class MainLayer: public Widgets::Layer {
public:
    explicit MainLayer(Widgets::Scene *parent);
};
}// namespace UI

#endif//ALGINE_EXAMPLES_MAINLAYER_H
