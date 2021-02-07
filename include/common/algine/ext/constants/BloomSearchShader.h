#ifndef ALGINE_BLOOMSEARCHSHADER_H
#define ALGINE_BLOOMSEARCHSHADER_H

#define constant(name, val) constexpr char name[] = val;

namespace algine {
    namespace BloomSearchShader {
        namespace Vars {
            constant(BaseImage, "image")
            constant(Threshold, "threshold")
            constant(BrightnessThreshold, "brightnessThreshold")
        }
    }
}

#undef constant

#endif //ALGINE_BLOOMSEARCHSHADER_H
