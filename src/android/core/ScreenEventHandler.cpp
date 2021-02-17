#include <algine/core/ScreenEventHandler.h>

namespace algine {
// so that you don't need to implement handling of each event

void ScreenEventHandler::pointerDown(float x, float y, int pointerId) {}
void ScreenEventHandler::pointerMove(float x, float y, int pointerId) {}
void ScreenEventHandler::pointerUp(float x, float y, int pointerId) {}
void ScreenEventHandler::pointerClick(float x, float y, int pointerId) {}

void ScreenEventHandler::surfaceResized(int width, int height) {}

void ScreenEventHandler::onResume() {}
void ScreenEventHandler::onPause() {}
void ScreenEventHandler::onDestroy() {}
}
