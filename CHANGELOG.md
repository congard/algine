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

1.3 beta-candidate:
 1. Optimization;
 2. Completely changed the architecture of the engine. Now everything is divided into separate modules;
 3. Removed `utils/shader_utils.cpp` and `utils/texture_utils.cpp`;
 4. All engine modules are placed in the namespace `algine`;
 5. `static GLuint loadTexture(const char *path)` and `class TextureArray` from `utils/texture_utils.cpp` moved into `texture.cpp`;
 6. `class TextureArray => struct TextureArray`;
 7. Removed many obsolete functions and structures (mainly from `algine.cpp`, `utils/texture_utils.cpp`, `utils/shader_utils.cpp`);
 8. Renamed most of the structures and their members. No more ugly names like `ALGINE_PATHS_TO_SHADERS`, `ALGINE_PROCESSED_SHADERS`;
 9. No more spam in the console when compiling shaders. Logs are displayed only when errors occur. The size of the log buffer is now strictly controlled, and exactly as needed to obtain this data;
 10. Some functions like `setVec3`, `setVec4`, `setMat4`, `pointer` from `utils/shader_utils.cpp` (`namespace su`) moved into `algine_renderer.cpp`; `void setVec3 && setVec4 && setMat4 > #define setVec3 && setVec4 && setMat4`;
 11. For compatibility with `GLM 0.9.9.0`, you must define `GLM_FORCE_CTOR_INIT`. Note: defined in `algine.cpp`;
 12. Fixed comments in the example (`main.cpp`);
 13. `algine_renderer.cpp`: used for rendering, has basic functions such as `setVec3`, `setVec4`, `setMat4`, `pointer`. Previously, `struct AlgineRenderer` was literally responsible for everything that multiplied by zero all the flexibility, customizability and embeddedness of the engine. Now it has only a few functions that are used for rendering. Such as: `renderQuad`, `mainPass`, `screenspacePass`, `bloomDofPass`, `bloomPass`,`dofPass`, `doubleBlendPass`, `blendPass`, `prepare`. No pointer-to-function!
 <br>`algine_shader_compiler.cpp`: used for generating Algine shaders, for compiling shaders and combining them into programs, for loading ids of objects from Algine shaders, for saving shaders. It has functions of reading and writing;
 <br>`algine_structs.cpp`: necessary for the operation of the engine structures
 <br>`algine.cpp`: just includes all engine and `glm` files
 <br>`constants.h`: engine constants (like `ALGINE_SSR_MODE_ENABLED`)
 <br>`framebuffer.cpp`: create, manage, delete framebuffer
 <br>`shaderprogram.cpp`: create, manage, delete shader and shader program; has functions for getting info log
 <br>`texture.cpp`: create, manage, delete texture;
 14. Fix in `model.cpp`: now it’s not necessary to specify paths to all textures. If the texture is not needed, its path can be `nullptr`;
 15. Arguments `internalformat = GL_RGB, format = GL_RGB, type = GL_UNSIGNED_BYTE` have been added to `loadTexture`;
 16. Added new effect: **SSR** (*Screen Space Reflections*). In this regard, introduced 2 new types of textures: *reflection strength map*, *jitter map*;
 17. Due to decentralization of engine functions, memory consumption is reduced;
 18. Other changes and improvements.
 
**If this new architecture is approved, then the process of creating documentation will begin, and the engine will move to the beta stage.**

1.4 beta-candidate:
 1. Algine now uses the [Assimp](https://github.com/assimp/assimp) - Open Asset Import Library. It can load 40+ model formats;
 2. Added support for materials. In addition to those that come with the model itself, which Assimp loads, additional materials have been added - AMTL - Algine Material Library. Which can be used simultaneously with the standard ones, but AMTL has a higher priority. AMTL is used to transfer parameters that are not usually provided by standard materials, such as `{ambient, diffuse, specular}Strength`, ` reflectionStrengthMap`, `jitterMap`; AMTL is a simple JSON array, which consists of JSON objects. Note that the AMTL name of the material must match the model name: `model.obj => model.amtl`;
 3. The `AlginePrograms` structure was divided into separate ones, such as `CShader`, `SShader`, `BLUShader`, `BLEShader`, `SSShader`;
 4. The extermination of ugly names continues: all the names in `*Shader` from `AlginePrograms` are now read easily, they look quite informative. **NO_CAPITAL_LETTERS**;
 5. Removed unused variables;
 6. Removed the enforced `applyDefaultTextureParams` from `Framebuffer::attachTexture`
 7. `fragment_shader.glsl`: `struct Mapping > Material`, removed unnecessary code; from `Lamp` removed variables which belongs to `Material`;
 8. `lamp.cpp`: removed variables which belongs to `material.cpp`;
 9. `algine.cpp => algine.h`;
 10. Removed support of CMF models. CMF was a temporary solution;
 11. Created `io.cpp` - reading/creating, absolute path, exists methods;
 12. Created `material.cpp` - material variables, loads AMTL;
 13. Created `mesh.cpp` - loads mesh, materials
 14. Some methods were renamed: now their name clearly reflects their essence;
 15. Added **Cinematic Depth Of Field**;
 16. Added methods for saving textures (2D, Cubemap), for getting texture data;
 17. Added [JSON library](https://github.com/nlohmann/json), [stb_image_write.h](https://github.com/nothings/stb);
 18. Updated [stb_image.h](https://github.com/nothings/stb);
 19. New example scene;
 20. Minor fixes, other changes and improvements.

1.4.1 beta-candidate:
 1. Fixed: shadow mapping not working on some GPUs (tested on Intel HD Graphics 520)
 2. Fixed: flickering on some GPUs (tested on Intel HD Graphics 520)
 3. Created `make.bat` and `run.bat` files for Windows, written instructions for compiling on Windows

1.4.2 beta-candidate:
 1. Fixed: black screen when shadow mapping enabled on AMD GPUs
 2. Fixed: make.sh not working on old debian systems
 3. Now shadow mapping by default without PCF

1.5 alpha:
 1. `Mesh::genBuffers` segmentation fault fix (if gen without normals/texCoords/etc)
 2. Removed src/utils dir. `math.cpp` replaced to src
 3. Removed unnecessary `#include` in some files
 4. Added `types.h` where defined basic data types, such as `int8`, `uint8`, `int16`, `uint16`, `uint` (32 bit), `int64`, `uint64`, `float64`. Also on the basis of the bit of your system are defined: `size` (`int` or `long`), `usize` (`unsigned int` or `unsigned long`)
 5. Renamed and changed: `struct ModelBuffers => struct Shape`, `class Model => struct Model`; `class Lamp => struct Light`, `class LampModel => struct Lamp` (it will be changed later)
 6. `make.sh` devided on `make.sh` and `run.sh` (for Linux systems)
 7. Removed unnecessary data from `fragment_shader.glsl`, `main.cpp`
 8. Added advanced FPS meter in example scene, new animated models (man and astroboy), exposure increased from 3.0 to 6.0, gamma from 1.0 to 1.125, cleanup in `main.cpp`
 9. Fixed chessboard texture (there were rainbow stripes)
 10. Added support for **skeletal animations**. Limitations on the *number of bones* / *number of bones per vertex* are hardware only.
 11. Added:
 <br>`animation.cpp`: animation structures
 <br>`bone.cpp`:  bone structures like `BoneInfo` (for engine only), `Bone`
 <br>`node.cpp`: node structures (only `Node`)
 12. Minor fixes, other changes and improvements.