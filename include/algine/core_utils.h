#ifndef ALGINE_CORE_UTILS_H
#define ALGINE_CORE_UTILS_H

#include <algine/types.h>
#include <vector>
#include <string>

#define delete_ptr(ptr) if (ptr != nullptr) { delete ptr; ptr = nullptr; }
#define delete_ptr_arr(ptr) if (ptr != nullptr) { delete[] ptr; ptr = nullptr; }

namespace algine {
// returns GPU Vendor name
// for example `NVIDIA Corporation`
const uchar* getGPUVendor();

// returns GPU Renderer name
// for example `GeForce 920MX/PCIe/SSE2`
const uchar* getGPURenderer();

// splits `in` by `delimiter`
std::vector<std::string> split(std::string &in, const std::string &delimiter);

// splits `in` by `delimiter`
std::vector<std::string> split(const std::string &in, const std::string &delimiter);

// returns shader string. `val` can be `ALGINE_VEC1..ALGINE_VEC4`, `ALGINE_SHADER_TEX_COMPONENT_RED..ALGINE_SHADER_TEX_COMPONENT_RGB`
std::string getShaderStr(const uint &val);

}

#endif /* ALGINE_CORE_UTILS_H */