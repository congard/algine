#ifndef ALGINE_BLENDBLOOMMODULE_H
#define ALGINE_BLENDBLOOMMODULE_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
    namespace BlendBloomModule {
        namespace Settings {
            constant(BloomAdd, "ALGINE_BLOOM_TYPE_ADD")
            constant(BloomScreen, "ALGINE_BLOOM_TYPE_SCREEN")
        }

        // TODO: tmp, BlendBloomModule shouldn't contains any Vars
        namespace Vars {
            constant(BaseImage, "image")
            constant(BloomImage, "bloom")
            constant(Exposure, "exposure")
            constant(Gamma, "gamma")
        }
    }
}

#undef constant

#endif //ALGINE_BLENDBLOOMMODULE_H
