#include <algine/core/Engine.h>

#ifdef ALGINE_QT_PLATFORM
    #include <algine/core/window/QtWindow.h>
#else
    #include <algine/core/window/GLFWWindow.h>
#endif

#include "MainContent.h"

using namespace algine;

static void exec() {
    Engine::setDPI(130);

#ifdef ALGINE_QT_PLATFORM
    auto window = new QtWindow();
    window->setDimensions(512, 512);

    window->addOnInitializedListener([window]() {
        window->setContent(new MainContent());
        window->renderLoop(1000);
    });

    window->show();
#else
    GLFWWindow window("BasicWidgets", 1366, 768);
    window.setFullscreenDimensions(1366, 768);
    window.setMouseTracking(true);
    window.setKeyboardTracking(true);
    window.setWindowStateTracking(true);
    //window.setCursorMode(Window::CursorMode::Disabled);
    window.setContentLater<MainContent>();
    window.renderLoop();
#endif
}

int main(int argc, char **argv) {
    Engine::exec(argc, argv, &exec);
    return 0;
}
