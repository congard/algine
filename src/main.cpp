/**
 * Algine: C++ OpenGL Engine
 *
 * My telegram: https://t.me/congard
 * My github: https://github.com/congard
 * @author congard
 */

// #define debug_sm
#define pcf // for soft shadows

#define GLM_FORCE_CTOR_INIT

#include <iostream>
#include <thread>
#include <chrono>

#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>

#define ALGINE_LOGGING

#include <algine/algine.h>

#define SHADOW_MAP_RESOLUTION 1024
#define bloomK 0.5f
#define bloomBlurAmount 4
#define bloomBlurKernelRadius 15
#define bloomBlurKernelSigma 16
#define dofBlurAmount 4
#define dofBlurKernelRadius 4
#define dofBlurKernelSigma 4
#define cocBlurKernelRadius 4
#define cocBlurKernelSigma 8

#define FULLSCREEN !true

#define POINT_LAMPS_COUNT 1
#define DIR_LAMPS_COUNT 1
#define SHAPES_COUNT 4
#define MODELS_COUNT 3

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(algine::MouseEventListener::MouseEvent *event);
void cursor_pos_callback(GLFWwindow* window, double x, double y);

// Window dimensions
GLuint winWidth = 1366, winHeight = 763;
GLFWwindow* window;

// matrices
glm::mat4 *modelMatrix; // model matrix stored in Model::transformation

// framebuffers, textures etc for rendering
using namespace algine;

// models
Shape shapes[SHAPES_COUNT];
Model models[MODELS_COUNT], lamps[POINT_LAMPS_COUNT + DIR_LAMPS_COUNT];
Animator manAnimator, astroboyAnimator; // animator for man, astroboy models

// light
PointLamp pointLamps[POINT_LAMPS_COUNT];
DirLamp dirLamps[DIR_LAMPS_COUNT];

// renderer
AlgineRenderer renderer;
CubeRenderer skyboxRenderer;
QuadRenderer quadRenderer;

GLuint
    displayFB,
    screenspaceFB,
    bloomSearchFB,
    pingpongFB[2],
    pingpongBlurBloomFB[2],
    pingpongBlurCoCFB[2],
    cocFB,
    rbo;

GLuint
    colorTex,
    normalTex,
    ssrValues,
    positionTex,
    screenspaceTex,
    bloomTex,
    pingpongDofTex[2],
    pingpongBlurTex[2],
    pingpongBlurCoCTex[2],
    skybox,
    cocTex;

CShader cs;
SShader ss, ss_dir;
DOFBlurShader dofBlurH, dofBlurV;
DOFCoCShader dofCoCShader;
BlurShader blurBloomH, blurBloomV, blurCoCH, blurCoCV;
BlendShader blendShader;
SSRShader ssrs;
BloomSearchShader bloomSearchShader;
CubemapShader skyboxShader;

BlurShader *blurBloomShaders[2] = { &blurBloomH, &blurBloomV };
BlurShader *blurCoCShaders[2] = { &blurCoCH, &blurCoCV };

AlgineParams params;
ColorShaderParams csp;
ShadowShaderParams ssp;
DOFBlurShaderParams dofBlurParams;

MouseEventListener mouseEventListener;

FPSCamera camera;
FPSCameraController camController;

float blendExposure = 6.0f, blendGamma = 1.125f;

float
    // DOF variables
    dofImageDistance = 1.0f,
    dofAperture = 10.0f,
    dof_max_sigma = 6.0f,
    dof_min_sigma = 0.0001f,
    bleedingEliminationMinDeltaZ = 5.0f,
    bleedingEliminationMinDeltaCoC = 1.0f,
    bleedingEliminationMaxFocusCoC = 2.5f,

    // diskRadius variables
    diskRadius_k = 1.0f / 25.0f,
    diskRadius_min = 0.0f,

    // shadow opacity: 1.0 - opaque shadow (by default), 0.0 - transparent
    shadowOpacity = 0.65f;

void updateRenderTextures() {
    cfgtex2D(colorTex, GL_RGB16F, GL_RGB, winWidth, winHeight);
    cfgtex2D(normalTex, GL_RGB16F, GL_RGB, winWidth, winHeight);
    cfgtex2D(ssrValues, GL_RG16F, GL_RG, winWidth, winHeight);
    cfgtex2D(positionTex, GL_RGB16F, GL_RGB, winWidth, winHeight);

    cfgtex2D(screenspaceTex, GL_RGB16F, GL_RGB, winWidth, winHeight);

    cfgtex2D(bloomTex, GL_RGB16F, GL_RGB, winWidth * bloomK, winHeight * bloomK);

    for (size_t i = 0; i < 2; i++) {
        cfgtex2D(pingpongBlurTex[i], GL_RGB16F, GL_RGB, winWidth * bloomK, winHeight * bloomK);
        cfgtex2D(pingpongDofTex[i], GL_RGB16F, GL_RGB, winWidth, winHeight);
        cfgtex2D(pingpongBlurCoCTex[i], GL_R16F, GL_RED, winWidth, winHeight);
    }

    cfgtex2D(cocTex, GL_R16F, GL_RED, winWidth, winHeight);
}

/**
 * To correctly display the scene when changing the window size
 */
void window_size_callback(GLFWwindow* window, int width, int height) {
    winWidth = width;
    winHeight = height;

    bindFramebuffer(displayFB);
    renderer.configureMainPassRenderbuffer(rbo, width, height);
    bindFramebuffer(0);

    updateRenderTextures();

    camera.aspectRatio = (float)winWidth / (float)winHeight;
    camera.perspective();
}

/**
 * Creates Lamp with default params
 */
void createPointLamp(PointLamp &result, const glm::vec3 &pos, const glm::vec3 &color, usize id) {
    result.pos = pos;
    result.color = color;
    result.kc = 1;
    result.kl = 0.045f;
    result.kq = 0.0075f;

    result.cs = &cs;
    result.ss = &ss;
    result.index = id;
    result.initShadowMapping(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

    glUseProgram(cs.programId);
    result.push_color();
    result.push_kc();
    result.push_kl();
    result.push_kq();
    result.push_far();
    result.push_bias();
    glUseProgram(0);
}

void createDirLamp(DirLamp &result, const glm::vec3 &pos, const glm::vec3 &color, usize id) {
    result.pos = pos;
    result.color = color;
    result.kc = 1;
    result.kl = 0.045f;
    result.kq = 0.0075f;

    result.cs = &cs;
    result.ss = &ss;
    result.index = id;
    result.textureStartId = 6 + POINT_LAMPS_COUNT;
    result.orthoShadowMapping(-10.0f, 10.0f, -10.0f, 10.0f);
    result.initShadowMapping(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

    glUseProgram(cs.programId);
    result.push_color();
    result.push_kc();
    result.push_kl();
    result.push_kq();
    result.push_lightMatrix();
    result.push_minBias();
    result.push_maxBias();
    glUseProgram(0);
}

void createShapes(const std::string &path, const uint params, const size_t id, const bool inverseNormals = false, const uint bonesPerVertex = 0) {
    shapes[id].bonesPerVertex = bonesPerVertex;
    shapes[id].init(path, params);
    if (inverseNormals) shapes[id].inverseNormals();
    shapes[id].genBuffers();
    shapes[id].createVAO(ss.inPosition, -1, -1, -1, -1, ss.inBoneWeights, ss.inBoneIds); // all shadow shaders have same ids
    shapes[id].createVAO(cs.inPosition, cs.inTexCoord, cs.inNormal, cs.inTangent, cs.inBitangent, cs.inBoneWeights, cs.inBoneIds);
}

/* init code begin */

/**
 * Init GL: creating window, etc.
 */
void initGL() {
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    // Init GLFW
    if (!glfwInit()) std::cout << "GLFW init failed";
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    window = FULLSCREEN ?
        glfwCreateWindow(winWidth, winHeight, "Algine", glfwGetPrimaryMonitor(), nullptr) :
        glfwCreateWindow(winWidth, winHeight, "Algine", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_NO_ERROR) std::cout << "GLEW init failed\n";

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
	glDepthMask(true);

    std::cout << "Your GPU vendor: " << getGPUVendor() << "\nYour GPU renderer: " << getGPURenderer() << "\n";
}

/**
 * Loading and compiling shaders
 */
void initShaders() {
    std::cout << "Compiling algine shaders\n";

    #ifdef debug_sm
    params.shadowMappingMode = ALGINE_SHADOW_MAPPING_MODE_DISABLED;
    #endif
    #ifdef pcf
    params.shadowMappingMode = ALGINE_SHADOW_MAPPING_MODE_ENABLED;
    #else
    params.shadowMappingMode = ALGINE_SHADOW_MAPPING_MODE_SIMPLE;
    #endif
    params.boneSystemMode = ALGINE_BONE_SYSTEM_ENABLED;
    dofBlurParams.blurKernelRadius = dofBlurKernelRadius;
    dofBlurParams.bleedingEliminationDeltaZ = ALGINE_ENABLED;
    dofBlurParams.bleedingEliminationDeltaCoC = ALGINE_ENABLED;
    dofBlurParams.bleedingEliminationFocusCoC = ALGINE_ENABLED;

    cs.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getCS(
        params, csp,
        "src/resources/shaders/vertex_shader.glsl",
        "src/resources/shaders/fragment_shader.glsl")
    ));
    ss.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getSS(
        params, ssp,
        "src/resources/shaders/shadow/vertex_shadow_shader.glsl",
        "src/resources/shaders/shadow/fragment_shadow_shader.glsl",
        "src/resources/shaders/shadow/geometry_shadow_shader.glsl")
    ));
    params.shadowMappingType = ALGINE_SHADOW_MAPPING_TYPE_DIR_LIGHTING;
    ss_dir.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getSS(
        params, ssp,
        "src/resources/shaders/shadow/vertex_shadow_shader.glsl",
        "src/resources/shaders/shadow/fragment_shadow_shader.glsl")
    ));
    ssrs.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getSSRShader(
        "src/resources/shaders/basic/quad_vertex.glsl",
        "src/resources/shaders/ssr/fragment.glsl")
    ));

    dofBlurParams.type = ALGINE_DOF_FROM_COC_MAP;
    std::vector<ShadersData> blus = scompiler::getDOFBlurShader(
        dofBlurParams,
        "src/resources/shaders/basic/quad_vertex.glsl",
        "src/resources/shaders/dof/fragment.glsl"
    );
    dofBlurH.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(
        blus[0]
    ));
    dofBlurV.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(
        blus[1]
    ));

    dofBlurParams.type = ALGINE_CINEMATIC_DOF;
    dofCoCShader.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getDOFCoCShader(
        dofBlurParams,
        "src/resources/shaders/basic/quad_vertex.glsl",
        "src/resources/shaders/dof/coc_fragment.glsl")
    ));

    blendShader.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getBlendShader(
        params,
        "src/resources/shaders/basic/quad_vertex.glsl",
        "src/resources/shaders/blend/fragment.glsl")
    ));

    bloomSearchShader.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getBloomSearchShader(
        "src/resources/shaders/basic/quad_vertex.glsl",
        "src/resources/shaders/bloom/fragment_search.glsl")
    ));

    std::vector<ShadersData> blur = scompiler::getBlurShader(
        BlurShaderParams { bloomBlurKernelRadius },
        "src/resources/shaders/basic/quad_vertex.glsl",
        "src/resources/shaders/blur/fragment.glsl"
    );
    blurBloomH.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(blur[0]));
    blurBloomV.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(blur[1]));

    blur = scompiler::getBlurShader(
        BlurShaderParams { cocBlurKernelRadius, ALGINE_VEC1, ALGINE_SHADER_TEX_COMPONENT_RED },
        "src/resources/shaders/basic/quad_vertex.glsl",
        "src/resources/shaders/blur/fragment.glsl"
    );
    blurCoCH.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(blur[0]));
    blurCoCV.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(blur[1]));

    CubemapShaderParams cubemapShaderParams;
    cubemapShaderParams.positionOutput = ALGINE_SPHERE_POSITIONS;
    skyboxShader.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getCubemapShader(
        cubemapShaderParams,
        "src/resources/shaders/basic/cubemap_vertex.glsl",
        "src/resources/shaders/basic/cubemap_fragment.glsl")
    ));

    std::cout << "Compilation done\n";

    scompiler::loadLocations(cs, csp);
    scompiler::loadLocations(ss);
    scompiler::loadLocations(ss_dir);
    scompiler::loadLocations(ssrs);
    scompiler::loadLocations(dofBlurH);
    scompiler::loadLocations(dofBlurV);
    scompiler::loadLocations(dofCoCShader);
    scompiler::loadLocations(blendShader);
    scompiler::loadLocations(bloomSearchShader);
    scompiler::loadLocations(blurBloomH);
    scompiler::loadLocations(blurBloomV);
    scompiler::loadLocations(blurCoCH);
    scompiler::loadLocations(blurCoCV);
    scompiler::loadLocations(skyboxShader);

    renderer.ssrs = &ssrs;
    renderer.blendShader = &blendShader;
    renderer.dofBlurShaders[0] = &dofBlurH;
    renderer.dofBlurShaders[1] = &dofBlurV;
    renderer.bloomSearchShader = &bloomSearchShader;
    renderer.blurShaders = blurBloomShaders;
    renderer.dofCoCShader = &dofCoCShader;
    renderer.quadRenderer = &quadRenderer;

    skyboxRenderer.init(skyboxShader.inPosition);
    quadRenderer.init(ALGINE_IN_POS_LOCATION, ALGINE_IN_TEX_COORD_LOCATION);

    Framebuffer::create(&displayFB);
    Framebuffer::create(&screenspaceFB);
    Framebuffer::create(&bloomSearchFB);
    Framebuffer::create(pingpongFB, 2);
    Framebuffer::create(pingpongBlurBloomFB, 2);
    Framebuffer::create(pingpongBlurCoCFB, 2);
    Framebuffer::create(&cocFB);

    Renderbuffer::create(&rbo);

    Texture::create(&colorTex);
    Texture::create(&normalTex);
    Texture::create(&ssrValues);
    Texture::create(&positionTex);
    Texture::create(&screenspaceTex);
    Texture::create(&bloomTex);
    Texture::create(pingpongDofTex, 2);
    Texture::create(pingpongBlurTex, 2);
    Texture::create(pingpongBlurCoCTex, 2);
    Texture::create(&cocTex);

    TextureCube::create(&skybox);
    TextureCube::CubePaths paths(
        "src/resources/skybox/right.tga", "src/resources/skybox/left.tga",
        "src/resources/skybox/top.jpg", "src/resources/skybox/bottom.tga",
        "src/resources/skybox/back.tga", "src/resources/skybox/front.tga"
    );
    TextureParams params;
    params.internalformat = GL_RGB;
    params.format = GL_RGB;
    params.type = GL_UNSIGNED_BYTE;
    bindTextureCube(skybox);
    TextureCube::loadFaces(params, paths);
    applyDefaultTextureCubeParams();
    bindTextureCube(0);

    applyDefaultTexture2DParams(colorTex);
    applyDefaultTexture2DParams(normalTex);
    applyDefaultTexture2DParams(ssrValues);
    applyDefaultTexture2DParams(positionTex);
    applyDefaultTexture2DParams(screenspaceTex);
    applyDefaultTexture2DParams(bloomTex);
    applyDefaultTexture2DParams(pingpongDofTex, 2);
    applyDefaultTexture2DParams(pingpongBlurTex, 2);
    applyDefaultTexture2DParams(pingpongBlurCoCTex, 2);
    applyDefaultTexture2DParams(cocTex);

    updateRenderTextures();

    GLuint displayColorAttachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    bindFramebuffer(displayFB);
    renderer.configureMainPassRenderbuffer(rbo, winWidth, winHeight);
    glDrawBuffers(4, displayColorAttachments);

    Framebuffer::attachTexture2D(colorTex, COLOR_ATTACHMENT(0));
    Framebuffer::attachTexture2D(normalTex, COLOR_ATTACHMENT(1));
    Framebuffer::attachTexture2D(positionTex, COLOR_ATTACHMENT(2));
    Framebuffer::attachTexture2D(ssrValues, COLOR_ATTACHMENT(3));

    for (size_t i = 0; i < 2; i++) {
        // configuring ping-pong (blur)
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFB[i]);
        Framebuffer::attachTexture2D(pingpongDofTex[i], COLOR_ATTACHMENT(0));

        // configuring ping-pong (blur)
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongBlurBloomFB[i]);
        Framebuffer::attachTexture2D(pingpongBlurTex[i], COLOR_ATTACHMENT(0));

        // configuring ping-pong (blur CoC)
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongBlurCoCFB[i]);
        Framebuffer::attachTexture2D(pingpongBlurCoCTex[i], COLOR_ATTACHMENT(0));
    }

    // sending to shader center of kernel and right part
    float kernelBloom[bloomBlurKernelRadius * 2 - 1];
    float kernelCoC[cocBlurKernelRadius * 2 - 1];
    getGB1DKernel(kernelBloom, bloomBlurKernelRadius * 2 - 1, bloomBlurKernelSigma);
    getGB1DKernel(kernelCoC, cocBlurKernelRadius * 2 - 1, cocBlurKernelSigma);
    
    for (size_t j = 0; j < 2; j++) {
        glUseProgram(blurBloomShaders[j]->programId);
        for (uint i = 0; i < bloomBlurKernelRadius; i++) {
            glUniform1f(
                j ? blurBloomV.kernel + (bloomBlurKernelRadius - 1 - i) : blurBloomH.kernel + (bloomBlurKernelRadius - 1 - i),
                kernelBloom[i]
            );
        }

        glUseProgram(blurCoCShaders[j]->programId);
        for (uint i = 0; i < cocBlurKernelRadius; i++) {
            glUniform1f(
                j ? blurCoCV.kernel + (cocBlurKernelRadius - 1 - i) : blurCoCH.kernel + (cocBlurKernelRadius - 1 - i),
                kernelCoC[i]
            );
        }
    }

    glUseProgram(0);

    bindFramebuffer(screenspaceFB);
    Framebuffer::attachTexture2D(screenspaceTex, COLOR_ATTACHMENT(0));
    
    bindFramebuffer(bloomSearchFB);
    Framebuffer::attachTexture2D(bloomTex, COLOR_ATTACHMENT(0));

    bindFramebuffer(cocFB);
    Framebuffer::attachTexture2D(cocTex, COLOR_ATTACHMENT(0));
    bindFramebuffer(0);

    // configuring CS
    useShaderProgram(cs.programId);
    glUniform1i(cs.materialAmbientTex, 0);
    glUniform1i(cs.materialDiffuseTex, 1);
    glUniform1i(cs.materialSpecularTex, 2);
    glUniform1i(cs.materialNormalTex, 3);
    glUniform1i(cs.materialReflectionStrengthTex, 4);
    glUniform1i(cs.materialJitterTex, 5);
    glUniform1f(cs.shadowOpacity, shadowOpacity);

    // configuring CubemapShader
    useShaderProgram(skyboxShader.programId);
    setVec3(skyboxShader.color, glm::vec3(0.125f));
    glUniform1f(skyboxShader.positionScaling, 64.0f);

    // blend setting
    glUseProgram(blendShader.programId);
    glUniform1i(blendShader.samplerImage, 0);   // GL_TEXTURE0
    glUniform1i(blendShader.samplerBloom, 1); // GL_TEXTURE1
    glUniform1f(blendShader.exposure, blendExposure);
    glUniform1f(blendShader.gamma, blendGamma);

    // dof blur setting
    for (size_t i = 0; i < 2; i++) {
        glUseProgram(renderer.dofBlurShaders[i]->programId);
        glUniform1i(renderer.dofBlurShaders[i]->samplerImage, 0);
        glUniform1i(renderer.dofBlurShaders[i]->samplerCoCMap, 1);
        glUniform1i(renderer.dofBlurShaders[i]->samplerPositionMap, 2);
        glUniform1f(renderer.dofBlurShaders[i]->bleedingEliminationMinDeltaZ, bleedingEliminationMinDeltaZ);
        glUniform1f(renderer.dofBlurShaders[i]->bleedingEliminationMinDeltaCoC, bleedingEliminationMinDeltaCoC);
        glUniform1f(renderer.dofBlurShaders[i]->bleedingEliminationMaxFocusCoC, bleedingEliminationMaxFocusCoC);
    }

    // screen space setting
    glUseProgram(ssrs.programId);
    glUniform1i(ssrs.samplerNormalMap, 1);
    glUniform1i(ssrs.samplerSSRValuesMap, 2);
    glUniform1i(ssrs.samplerPositionMap, 3);
    useShaderProgram(0);
}

/**
 * Creating matrices
 */
void initCamera() {
    camera.pos = glm::vec3(0, 10, 14);
    camera.far = 64.0f;
    camera.fov = glm::radians(60.0f);
    camera.aspectRatio = (float)winWidth / (float)winHeight;
    camera.perspective();
    
    camera.pitch = glm::radians(30.0f);
    camera.updateView();
    
    camController.camera = &camera;
}

/**
 * Creating shapes and loading textures
 */
void initShapes() {
    uint params = aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices;
    std::string path = "src/resources/models/";

    // classic chess
    createShapes(path + "chess/Classic Chess small.obj", params, 0);
    shapes[0].loadTextures(path + "chess");

    // Japanese lamp
    createShapes(path + "japanese_lamp/japanese_lamp.obj", params, 1, true);
    shapes[1].loadTextures(path + "japanese_lamp");

    // animated man
    createShapes(path + "man/man.dae", params, 2, false, 4);
    shapes[2].loadTextures(path + "man");

    createShapes(path + "astroboy/astroboy_walk.dae", params, 3, false, 4);
    shapes[3].loadTextures(path + "astroboy");
}

/**
 * Creating models from created buffers and loaded textures
 */
void createModels() {
    // classic chess
    models[0].shape = &shapes[0];

    // animated man
    manAnimator = Animator(AnimShape(&shapes[2].animations, &shapes[2].bones, &shapes[2].globalInverseTransform, &shapes[2].rootNode));
    models[1].shape = &shapes[2];
    models[1].rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    models[1].translate(glm::vec3(-2.0f, 0.0f, 0.0f));
    models[1].transform();
    models[1].animator = &manAnimator;

    // animated astroboy
    astroboyAnimator = Animator(AnimShape(&shapes[3].animations, &shapes[3].bones, &shapes[3].globalInverseTransform, &shapes[3].rootNode));
    models[2].shape = &shapes[3];
    models[2].rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    models[2].scale(glm::vec3(50.0f));
    models[2].translate(glm::vec3(2.0f, 0.0f, 0.0f));
    models[2].transform();
    models[2].animator = &astroboyAnimator;
}

/**
 * Creating light sources
 */
void initLamps() {
    createPointLamp(pointLamps[0], glm::vec3(0.0f, 8.0f, 15.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0);
    lamps[0].shape = &shapes[1];
    pointLamps[0].mptr = &lamps[0];
    lamps[0].translate(pointLamps[0].pos);
    lamps[0].transform();

    createDirLamp(dirLamps[0], glm::vec3(0.0f, 8.0f, -15.0f), glm::vec3(253.0f / 255.0f, 184.0f / 255.0f, 19.0f / 255.0f), 0);
    lamps[1].shape = &shapes[1];
    dirLamps[0].mptr = &lamps[1];
    lamps[1].translate(dirLamps[0].pos);
    lamps[1].transform();
}

/**
 * Binds to depth cubemaps
 */
void initShadowMaps() {
    glUseProgram(cs.programId);
    // to avoid black screen on AMD GPUs and old Intel HD Graphics
    for (usize i = 0; i < csp.maxPointLightsCount; i++) {
        glUniform1i(cs.pointLights[i].shadowMap, pointLamps[0].textureStartId + i);
		glActiveTexture(GL_TEXTURE0 + pointLamps[0].textureStartId + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    for (usize i = 0; i < POINT_LAMPS_COUNT; i++) pointLamps[i].push_shadowMap();

    // to avoid black screen on AMD GPUs and old Intel HD Graphics
    for (usize i = 0; i < csp.maxDirLightsCount; i++) {
        glUniform1i(cs.dirLights[i].shadowMap, dirLamps[0].textureStartId + i);
		glActiveTexture(GL_TEXTURE0 + dirLamps[0].textureStartId + i);
		glBindTexture(GL_TEXTURE_2D, 0);
    }
    for (usize i = 0; i < DIR_LAMPS_COUNT; i++) dirLamps[i].push_shadowMap();
    glUseProgram(0);
}

void initShadowCalculation() {
    glUseProgram(cs.programId);
    glUniform1f(cs.shadowDiskRadiusK, diskRadius_k);
    glUniform1f(cs.shadowDiskRadiusMin, diskRadius_min);
    glUseProgram(0);
}

/**
 * Initialize Depth of field
 */
void initDOF() {
    useShaderProgram(dofCoCShader.programId);
    glUniform1f(dofCoCShader.sigmaMax, dof_max_sigma);
    glUniform1f(dofCoCShader.sigmaMin, dof_min_sigma);
    glUniform1f(dofCoCShader.cinematicDOFAperture, dofAperture);
    glUniform1f(dofCoCShader.cinematicDOFImageDistance, dofImageDistance);
    glUniform1f(dofCoCShader.cinematicDOFPlaneInFocus, -1.0f);
    useShaderProgram(0);
}

/* init code end */

/**
 * Cleans memory before exit
 */
void recycleAll() {
    for (size_t i = 0; i < SHAPES_COUNT; i++) shapes[i].recycle();

    Framebuffer::destroy(&displayFB);
    Framebuffer::destroy(&screenspaceFB);
    Framebuffer::destroy(&bloomSearchFB);
    Framebuffer::destroy(pingpongFB, 2);
    Framebuffer::destroy(pingpongBlurBloomFB, 2);
    Framebuffer::destroy(pingpongBlurCoCFB, 2);
    Framebuffer::destroy(&cocFB);

    Texture::destroy(&colorTex);
    Texture::destroy(&normalTex);
    Texture::destroy(&ssrValues);
    Texture::destroy(&positionTex);
    Texture::destroy(&screenspaceTex);
    Texture::destroy(&bloomTex);
    Texture::destroy(pingpongDofTex, 2);
    Texture::destroy(pingpongBlurTex, 2);
    Texture::destroy(pingpongBlurCoCTex, 2);
    Texture::destroy(&skybox);
    Texture::destroy(&cocTex);

    Renderbuffer::destroy(&rbo);
}

void sendLampsData() {
	glUniform1i(cs.pointLightsCount, POINT_LAMPS_COUNT); // point lamps count
    glUniform1i(cs.dirLightsCount, DIR_LAMPS_COUNT); // dir lamps count
    setVec3(cs.viewPos, camera.pos); // current camera position
	for (size_t i = 0; i < POINT_LAMPS_COUNT; i++) pointLamps[i].push_pos();
    for (size_t i = 0; i < DIR_LAMPS_COUNT; i++) dirLamps[i].push_pos();
}

/* --- matrices --- */
glm::mat4 getPVMMatrix() {
    return camera.projection * camera.view * *modelMatrix;
}

glm::mat4 getVMMatrix() {
    return camera.view * *modelMatrix;
}

void updateMatrices() {
    setMat4(cs.matPVM, getPVMMatrix());
    setMat4(cs.matVM, getVMMatrix());
    setMat4(cs.matModel, *modelMatrix);
    setMat4(cs.matView, camera.view);
}

/**
 * Draws model in depth map
 * @param model
 * @param ss
 */
void drawModelDM(const Model &model, const SShader &ss) {
    glBindVertexArray(model.shape->vaos[0]);

    if (model.shape->bonesPerVertex != 0) {
        for (size_t i = 0; i < model.shape->bones.size(); i++) {
            setMat4(ss.bones + i, model.shape->bones[i].finalTransformation);
        }
    }

    glUniform1i(ss.boneAttribsPerVertex, model.shape->bonesPerVertex / 4 + (model.shape->bonesPerVertex % 4 == 0 ? 0 : 1));
    setMat4(ss.matModel, model.transformation);
    
    for (size_t i = 0; i < model.shape->meshes.size(); i++) {
        glDrawElements(GL_TRIANGLES, model.shape->meshes[i].count, GL_UNSIGNED_INT, reinterpret_cast<void*>(model.shape->meshes[i].start * sizeof(uint)));
    }
}

/**
 * Draws model
 */
void drawModel(Model &model) {
    glBindVertexArray(model.shape->vaos[1]);
    
    if (model.shape->bonesPerVertex != 0) {
        for (size_t i = 0; i < model.shape->bones.size(); i++) {
            setMat4(cs.bones + i, model.shape->bones[i].finalTransformation);
        }
    }

    glUniform1i(cs.boneAttribsPerVertex, model.shape->bonesPerVertex / 4 + (model.shape->bonesPerVertex % 4 == 0 ? 0 : 1));
    modelMatrix = &model.transformation;
	updateMatrices();
    for (size_t i = 0; i < model.shape->meshes.size(); i++) {
        texture2DAB(0, model.shape->meshes[i].mat.ambientTexture);
        texture2DAB(1, model.shape->meshes[i].mat.diffuseTexture);
        texture2DAB(2, model.shape->meshes[i].mat.specularTexture);
        texture2DAB(3, model.shape->meshes[i].mat.normalTexture);
        texture2DAB(4, model.shape->meshes[i].mat.reflectionTexture);
        texture2DAB(5, model.shape->meshes[i].mat.jitterTexture);

        glUniform1f(cs.materialAmbientStrength, model.shape->meshes[i].mat.ambientStrength);
        glUniform1f(cs.materialDiffuseStrength, model.shape->meshes[i].mat.diffuseStrength);
        glUniform1f(cs.materialSpecularStrength, model.shape->meshes[i].mat.specularStrength);
        glUniform1f(cs.materialShininess, model.shape->meshes[i].mat.shininess);

        glDrawElements(GL_TRIANGLES, model.shape->meshes[i].count, GL_UNSIGNED_INT, reinterpret_cast<void*>(model.shape->meshes[i].start * sizeof(uint)));
    }
}

/**
 * Renders to depth cubemap
 */
void renderToDepthCubemap(uint index) {
	pointLamps[index].begin();
	pointLamps[index].setLightPosSS();
	pointLamps[index].updateMatrices();
	pointLamps[index].setShadowMatricesSS();
	glClear(GL_DEPTH_BUFFER_BIT);

	// drawing models
	for (size_t i = 0; i < MODELS_COUNT; i++) drawModelDM(models[i], ss);

	// drawing lamps
	for (GLuint i = 0; i < POINT_LAMPS_COUNT; i++) {
		if (i == index) continue;
		drawModelDM(*pointLamps[i].mptr, ss);
	}

	pointLamps[index].end();
}

/**
 * Renders to depth map
 */
void renderToDepthMap(uint index) {
	dirLamps[index].begin();
    setMat4(ss_dir.matLightSpace, dirLamps[index].lightSpace);
	glClear(GL_DEPTH_BUFFER_BIT);

	// drawing models
	for (size_t i = 0; i < MODELS_COUNT; i++) drawModelDM(models[i], ss_dir);

	// drawing lamps
	for (GLuint i = 0; i < DIR_LAMPS_COUNT; i++) {
		if (i == index) continue;
		drawModelDM(*dirLamps[i].mptr, ss_dir);
	}

	dirLamps[index].end();
}

/**
 * Color rendering
 */
uint colorAttachment02[3] = { GL_COLOR_ATTACHMENT0, GL_NONE, GL_COLOR_ATTACHMENT2 };
uint colorAttachment0123[4] = {
    GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
};
void render() {
    renderer.mainPass(displayFB, rbo);
    
	// view port to window size
	glViewport(0, 0, winWidth, winHeight);
	// updating view matrix (because camera position was changed)
    // createViewMatrix();

    // render skybox
    glDrawBuffers(3, colorAttachment02);
    glDepthFunc(GL_LEQUAL);
    useShaderProgram(skyboxShader.programId);
    glUniformMatrix3fv(skyboxShader.matView, 1, GL_FALSE, glm::value_ptr(glm::mat3(camera.view)));
    setMat4(skyboxShader.matTransform, camera.projection * glm::mat4(glm::mat3(camera.view)));
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
    skyboxRenderer.render();

    glDepthFunc(GL_LESS);
    glDrawBuffers(4, colorAttachment0123);
    glUseProgram(cs.programId);

    // sending lamps parameters to fragment shader
	sendLampsData();
    
    // drawing
    for (size_t i = 0; i < MODELS_COUNT; i++) drawModel(models[i]);
	for (size_t i = 0; i < POINT_LAMPS_COUNT + DIR_LAMPS_COUNT; i++) drawModel(lamps[i]);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    renderer.quadRenderer->bindVAO();

    renderer.screenspacePass(screenspaceFB, colorTex, normalTex, ssrValues, positionTex);

    glViewport(0, 0, winWidth * bloomK, winHeight * bloomK);
    renderer.bloomSearchPass(bloomSearchFB, screenspaceTex);
    renderer.blurShaders = blurBloomShaders;
    renderer.blurPass(pingpongBlurBloomFB, pingpongBlurTex, bloomTex, bloomBlurAmount);
    glViewport(0, 0, winWidth, winHeight);

    renderer.dofCoCPass(cocFB, positionTex);

    renderer.blurShaders = blurCoCShaders;
    renderer.blurPass(pingpongBlurCoCFB, pingpongBlurCoCTex, cocTex, bloomBlurAmount);

    renderer.dofBlurPass(pingpongFB, pingpongDofTex, screenspaceTex, pingpongBlurCoCTex[!renderer.horizontal], positionTex, dofBlurAmount);

    bindFramebuffer(0);
    renderer.doubleBlendPass(pingpongDofTex[!renderer.horizontal], pingpongBlurTex[!renderer.horizontal]);
}

void display() {
    // animate
    for (usize i = 0; i < MODELS_COUNT; i++)
        if (models[i].shape->bonesPerVertex != 0)
            models[i].animator->animate(glfwGetTime());

    /* --- shadow rendering --- */
    // point lights
    glUseProgram(ss.programId);
	for (uint i = 0; i < POINT_LAMPS_COUNT; i++) {
        glUniform1f(ss.far, pointLamps[i].far);
        renderToDepthCubemap(i);
    }

    // dir lights
    glUseProgram(ss_dir.programId);
	for (uint i = 0; i < DIR_LAMPS_COUNT; i++)
        renderToDepthMap(i);
	
    glUseProgram(ssrs.programId);
    setMat4(ssrs.matProjection, camera.projection);
    setMat4(ssrs.matView, camera.view);

	/* --- color rendering --- */
	glClear(GL_DEPTH_BUFFER_BIT); // color will cleared by quad rendering
	render();
	glUseProgram(0);
}

void animate_scene() {
    glm::mat3 rotate = glm::mat3(glm::rotate(glm::mat4(), glm::radians(0.01f), glm::vec3(0, 1, 0)));
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        pointLamps[0].setPos(pointLamps[0].pos * rotate);
    }
}

// The MAIN function, from here we start the application and run the game loop
int main() {
    initGL();
    initShaders();
    initCamera();
    initShapes();
    createModels();
    initLamps();
    initShadowMaps();
    initShadowCalculation();
    initDOF();
    
    mouseEventListener.setCallback(mouse_callback);

    #ifndef debug_sm
    std::thread animate_scene_th(animate_scene);
    #endif

    double currentTime, previousTime = glfwGetTime();
    size_t frameCount = 0;
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        frameCount++;
        // If a second has passed.
        if (currentTime - previousTime >= 1.0) {
            // Display the average frame count and the average time for 1 frame
            std::cout << frameCount << " (" << (frameCount / (currentTime - previousTime)) << ") FPS, " << ((currentTime - previousTime) / frameCount) * 1000 << " ms\n";
            frameCount = 0;
            previousTime = currentTime;
        }

        display();
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    #ifndef debug_sm
    animate_scene_th.detach();
    #endif

    recycleAll();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    
    std::cout << "Exit with exit code 0\n";

    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
    else if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_RELEASE)) {
        camController.goForward();
        camera.updateView();
    } else if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_RELEASE)) {
        camController.goBack();
        camera.updateView();
    } else if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_RELEASE)) {
        camController.goLeft();
        camera.updateView();
    } else if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_RELEASE)) {
        camController.goRight();
        camera.updateView();
    }
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    double x;
    double y;
    glfwGetCursorPos(window, &x, &y);
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        mouseEventListener.buttonDown(x, y, MouseEventListener::ButtonRight);
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        mouseEventListener.buttonUp(x, y, MouseEventListener::ButtonRight);
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouseEventListener.buttonDown(x, y, MouseEventListener::ButtonLeft);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouseEventListener.buttonUp(x, y, MouseEventListener::ButtonLeft);
    }
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
    mouseEventListener.mouseMove(x, y);
}

void mouse_callback(MouseEventListener::MouseEvent *event) {
    switch(event->action) {
        case MouseEventListener::ActionDown:
            camController.setMousePos(event->getX(), event->getY());
            break;
        case MouseEventListener::ActionMove:
            if (!event->listener->buttons[MouseEventListener::ButtonLeft].isPressed)
                break;
            
            camController.mouseMove(event->getX(), event->getY());
            camera.updateView();
            break;
        case MouseEventListener::ActionLongClick:
            std::cout << "Long click " << event->button << "\n";
            break;
        case MouseEventListener::ActionClick:
            std::cout << "x: " << event->getX() << "; y: " << event->getY() << "\n";
        
            GLfloat *pixels = getPixels(positionTex, (size_t)event->getX(), winHeight - (size_t)event->getY(), 1, 1, GL_RGB);
        
            std::cout << "x: " << pixels[0] << "; y: " << pixels[1] << "; z: " << pixels[2] << "\n";
        
            useShaderProgram(dofCoCShader.programId);
            glUniform1f(dofCoCShader.cinematicDOFPlaneInFocus, pixels[2] == 0 ? FLT_EPSILON : pixels[2]);
            useShaderProgram(0);
        
            delete[] pixels;
            break;
    }
}
