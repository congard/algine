#ifndef ALGINE_SCREEN_H
#define ALGINE_SCREEN_H

#include <algine/core/Surface.h>

#include <string_view>

namespace algine {
class Screen: public Surface {
public:
    enum class ToastLength {
        Short = 0,
        Long = 1
    };

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

    static void showToast(std::string_view text, ToastLength length = ToastLength::Short);

    // this function is for internal calls only!
    // do not call it!
    static void __resized(int width, int height);

private:
    static Screen *m_screen;
};
}

#endif //ALGINE_SCREEN_H
