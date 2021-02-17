#ifndef ALGINE_SCREENEVENTHANDLER_H
#define ALGINE_SCREENEVENTHANDLER_H

#include <algine/core/EventHandler.h>

namespace algine {
class ScreenEventHandler: public EventHandler {
public:
    virtual void pointerDown(float x, float y, int pointerId);
    virtual void pointerMove(float x, float y, int pointerId);
    virtual void pointerUp(float x, float y, int pointerId);
    virtual void pointerClick(float x, float y, int pointerId);

    virtual void surfaceResized(int width, int height);

    virtual void onResume();
    virtual void onPause();
    virtual void onDestroy();
};
}

#endif //ALGINE_SCREENEVENTHANDLER_H
