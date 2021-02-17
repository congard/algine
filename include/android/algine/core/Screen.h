#ifndef ALGINE_SCREEN_H
#define ALGINE_SCREEN_H

#include <algine/core/Surface.h>

namespace algine {
class Screen: public Surface {
public:
    Screen();

    void renderFrame();
    void renderLoop() override;

    void stopRenderLoop() override;

    bool isRenderLoopRunning() const;
    bool isActionHandlerAvailable() const;

    bool isPointerAvailable(int pointerId) const;
    float getPointerX(int pointerId) const;
    float getPointerY(int pointerId) const;

    glm::vec2 getPointerPos(int pointerId) const;

    static inline auto instance() {
        return m_screen;
    }

    static void destroyInstance();

    // this function is for internal calls only!
    // do not call it!
    static void __resized(int width, int height);

private:
    static Screen *m_screen;
};
}

#endif //ALGINE_SCREEN_H
