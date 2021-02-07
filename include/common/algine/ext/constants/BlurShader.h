#ifndef ALGINE_BLUR_H_CONST
#define ALGINE_BLUR_H_CONST

#define constant(name, val) constexpr char name[] = val;

namespace algine {
    namespace BlurShader {
        namespace Settings {
            constant(OutputType, "vecout")
            constant(TexComponent, "texComponent")

            constant(Vertical, "ALGINE_VERTICAL")
            constant(Horizontal, "ALGINE_HORIZONTAL")
            constant(KernelRadius, "KERNEL_RADIUS")
        }

        namespace Vars {
            constant(BaseImage, "image")
            constant(Kernel, "kernel[0]")
        }
    }
}

#undef constant

#endif //ALGINE_BLUR_H_CONST
