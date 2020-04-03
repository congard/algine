#ifndef ALGINE_BLENDBLOOM_H
#define ALGINE_BLENDBLOOM_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
namespace Module {
namespace BlendBloom {
    namespace Settings {
        constant(BloomAdd, "ALGINE_BLOOM_TYPE_ADD")
        constant(BloomScreen, "ALGINE_BLOOM_TYPE_SCREEN")
    }
}
}
}

#undef constant

#endif //ALGINE_BLENDBLOOM_H
