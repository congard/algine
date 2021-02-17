#ifndef ALGINE_BRIDGE_H
#define ALGINE_BRIDGE_H

#include <glm/vec2.hpp>

namespace AndroidBridge {
void renderLoop();
void stopRenderLoop();

bool isRenderLoopRunning();
bool isActionHandlerAvailable();

bool isPointerAvailable(int pointerId);
float getPointerX(int pointerId);
float getPointerY(int pointerId);

glm::vec2 getPointerPos(int pointerId);

glm::ivec2 getViewDimensions();
}

#endif //ALGINE_BRIDGE_H
