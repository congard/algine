#ifndef ALGINE_CORE_UTILS_H
#define ALGINE_CORE_UTILS_H

#include <algine/types.h>
#include <vector>
#include <string>

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

}

#endif /* ALGINE_CORE_UTILS_H */