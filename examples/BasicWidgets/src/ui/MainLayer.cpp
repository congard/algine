#include "MainLayer.h"

#include <algine/core/widgets/Container.h>
#include <algine/core/log/Log.h>
#include <algine/core/log/logger/glm.h>

#include <tulz/demangler.h>

namespace dmg = tulz::demangler;

namespace algine {
template<typename T, typename L>
Logger&& operator<<(L &&logger, const Point<T> &p) {
    auto tName = typeid(T).name();
    logger << "Point<" << dmg::demangle(tName) << "> {" << p.getX() << ", " << p.getY() << "}";
    return std::move(logger);
}
}

namespace UI {
MainLayer::MainLayer(Widgets::Scene *parent)
    : Widgets::Layer(parent)
{
    auto container = Widget::constructFromXMLFile<Container*>("BasicWidgets.xml", this);
    setContainer(container);

    container->findChild<Widget*>("label0")->setEventListener(Event::Click, [](Widget *widget, const Event &event) {
        auto v2pi = []<typename T>(const glm::vec<2, T> &v) -> PointI {
            return {(int) v.x, (int) v.y};
        };

        auto &info = event.getPointerInfo();

        Log::verbose("MainLayer") << "label0 has detected click:\n"
            << "label0 local coords: " << info.getPos() << "\n"
            << "parent local coords: " << widget->mapFromParent(v2pi(info.getPos()));
    });

    container->findChild<Widget*>("container1")->setEventListener(Event::Click, [](Widget*, const Event &event) {
        auto &info = event.getPointerInfo();
        Log::verbose("MainLayer") << "container1 has detected click:\n"
            << "container1 local coords: " << info.getPos();
    });
}
}// namespace ui