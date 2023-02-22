#include <algine/core/unified/UnifiedEventHandler.h>
#include <algine/core/Screen.h>

namespace algine {
void UnifiedEventHandler::pointerDown(float x, float y, int pointerId) {
    onPointerPress({x, y, pointerId});
}

void UnifiedEventHandler::pointerMove(float x, float y, int pointerId) {
    onPointerMove({x, y, pointerId});
}

void UnifiedEventHandler::pointerUp(float x, float y, int pointerId) {
    onPointerRelease({x, y, pointerId});
}

void UnifiedEventHandler::pointerClick(float x, float y, int pointerId) {
    onPointerClick({x, y, pointerId});
}

void UnifiedEventHandler::surfaceResized(int width, int height) {
    onSizeChanged(width, height);
}

void UnifiedEventHandler::onPause() {
    onFocusChanged({FocusInfo::Reason::AppPaused});
}

void UnifiedEventHandler::onResume() {
    onFocusChanged({FocusInfo::Reason::AppResumed});
}
}
