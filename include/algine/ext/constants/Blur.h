#ifndef ALGINE_BLUR_H_CONST
#define ALGINE_BLUR_H_CONST

#define constant(name, val) constexpr char name[] = val;

namespace algine {
    namespace constants {
        namespace Blur {
            constant(BaseImage, "image")
            constant(Kernel, "kernel[0]")

            constant(Vertical, "ALGINE_VERTICAL")
            constant(Horizontal, "ALGINE_HORIZONTAL")
            constant(KernelRadius, "KERNEL_RADIUS")
        }
    }
}

#undef constant
#endif //ALGINE_BLUR_H_CONST
