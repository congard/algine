#include <algine/gputils.h>
#include <GL/glew.h>
#include <chrono>

namespace algine {
// returns GPU Vendor name
// for example `NVIDIA Corporation`
const uchar* getGPUVendor() {
    return glGetString(GL_VENDOR);
}

// returns GPU Renderer name
// for example `GeForce 920MX/PCIe/SSE2`
const uchar* getGPURenderer() {
    return glGetString(GL_RENDERER);
}

long getTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

}
