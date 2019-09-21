#include <algine/core_utils.h>
#include <GL/glew.h>
#include <iostream>
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

// splits `in` by `delimiter`
std::vector<std::string> split(std::string &in, const std::string &delimiter) {
    std::vector<std::string> result;

    size_t pos = 0;
    std::string token;
    while ((pos = in.find(delimiter)) != std::string::npos) {
        token = in.substr(0, pos);
        result.push_back(token);
        in.erase(0, pos + delimiter.length());
    }
    result.push_back(in);

    return result;
}

// splits `in` by `delimiter`
std::vector<std::string> split(const std::string &in, const std::string &delimiter) {
    std::string _in = in;
    return split(_in, delimiter);
}

// replaces first found occurrence
std::string replace(const std::string &src, const std::string &target, const std::string &replacement) {
    std::string _src = src;
    std::size_t pos = _src.find(target);
    if (pos == std::string::npos) return _src;
    return _src.replace(pos, target.length(), replacement);
}

long getTime() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

}
