# Algine changelog
1.0 alpha:
 1. Project created

1.1 alpha:
 1. Added Depth of field suuport
 2. Added large models support
 3. Added scene management: key **Q** changes `focalDepth` on **+1**, **A** changes `focalDepth` on **-1**, **W** changes `focalRange` on **+5**, **S** changes `focalRange` on **-5**
 4. Removing unnecessary code (in `advanced_rendering.cpp`)
 5. Some optimization (in `fragment_blur_shader.glsl`, `fragment_blend_shader.glsl` and `advanced_rendering.cpp`)
 6. Added support for arguments to the compilation-run script (`make.sh`)
 7. Fixed `recycle()` in `advanced_rendering.cpp`: added removal of `quadBuffers`