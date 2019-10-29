#ifndef ALGINE_GPUTILS_H
#define ALGINE_GPUTILS_H

#include <algine/types.h>
#include <vector>
#include <string>
#include <regex>

namespace algine {
// returns GPU Vendor name
// for example `NVIDIA Corporation`
const uchar* getGPUVendor();

// returns GPU Renderer name
// for example `GeForce 920MX/PCIe/SSE2`
const uchar* getGPURenderer();

// returns current time in milliseconds
long getTime();

}

#endif /*ALGINE_GPUTILS_H*/
