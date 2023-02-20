#include <algine/core/x/XEventHandler.h>
#include <algine/core/Screen.h>

namespace algine {
void XEventHandler::pointerDown(float x, float y, int pointerId) {
    onPointerPress({x, y, pointerId});
}

void XEventHandler::pointerMove(float x, float y, int pointerId) {
    onPointerMove({x, y, pointerId});
}

void XEventHandler::pointerUp(float x, float y, int pointerId) {
    onPointerRelease({x, y, pointerId});
}

void XEventHandler::pointerClick(float x, float y, int pointerId) {
    onPointerClick({x, y, pointerId});
}

void XEventHandler::surfaceResized(int width, int height) {
    onSizeChanged(width, height);
}

void XEventHandler::onPause() {
    onFocusChanged({FocusInfo::Reason::AppPaused});
}

void XEventHandler::onResume() {
    onFocusChanged({FocusInfo::Reason::AppResumed});
}
}
