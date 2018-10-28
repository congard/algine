# Algine changelog
1.0 alpha:
 1. Project created

1.1 alpha:
 1. Added Depth of field support
 2. Added large models support
 3. Added scene management: key **Q** changes `focalDepth` on **+1**, **A** changes `focalDepth` on **-1**, **W** changes `focalRange` on **+5**, **S** changes `focalRange` on **-5**
 4. Removing unnecessary code (in `advanced_rendering.cpp`)
 5. Some optimization (in `fragment_blur_shader.glsl`, `fragment_blend_shader.glsl` and `advanced_rendering.cpp`)
 6. Added support for arguments to the compilation-run script (`make.sh`)
 7. Fixed `recycle()` in `advanced_rendering.cpp`: added removal of `quadBuffers`

1.2 alpha:
 1. Optimization
 2. Added ability to manage effects (bloom, dof etc) (enable and disable, you must perform before compiling shaders; all effects are activated by default)
 3. Removed `advanced_rendering.cpp`, all functionality transferred to file `algine.cpp`, `struct AlgineRenderer`
 4. More flexible shader configuration
 5. Preloading locations from shaders
 6. `class ShaderProgram => struct ShaderProgram`
 7. Compressed textures for models
 8. Added FPS meter in the sample scene
 9. Other improvements and fixes