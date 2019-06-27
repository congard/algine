/**
 * Algine: C++ OpenGL Engine
 *
 * My telegram: https://t.me/congard
 * My github: https://github.com/congard
 * @author congard
 */

// #define debug_sm
#define pcf // for soft shadows

#include <iostream>
#include <thread>
#include <chrono>

#define ALGINE_LOGGING

#include "algine.h"

#define SHADOW_MAP_RESOLUTION 1024
#define bloomK 0.5f
#define bloomBlurAmount 4
#define bloomBlurKernelRadius 15
#define bloomBlurKernelSigma 16
#define dofBlurAmount 4
#define dofBlurKernelRadius 4
#define dofBlurKernelSigma 4

#define FULLSCREEN !true

#define POINT_LAMPS_COUNT 1
#define DIR_LAMPS_COUNT 1
#define SHAPES_COUNT 4
#define MODELS_COUNT 3

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);

// Window dimensions
GLuint winWidth = 1366, winHeight = 763;
GLFWwindow* window;

GLuint vao;

// matrices
glm::mat4 projectionMatrix, viewMatrix, *modelMatrix; // model matrix stored in Model::transformation

// camera params
glm::vec3 cameraPos(-0, 8, 7);
glm::vec3 cameraLookAt(0, 4, 0);
glm::vec3 cameraUp(0, 1, 0);

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
CubemapRenderer skyboxRenderer;

GLuint
    displayFB,
    screenspaceFB,
    bloomSearchFB,
    pingpongFB[2],
    pingpongBlurFB[2],
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
    skybox,
    cocTex;

CShader cs;
SShader ss, ss_dir;
DOFBlurShader dofBlurH, dofBlurV;
DOFCoCShader dofCoCShader;
BlurShader blurH, blurV;
BlendShader blendShader;
SSRShader ssrs;
BloomSearchShader bloomSearchShader;
CubemapShader skyboxShader;

AlgineParams params;
ColorShaderParams csp;
ShadowShaderParams ssp;

float exposure = 6.0f, gamma = 1.125f;

float
    // DOF variables
    dofImageDistance = 1.0f,
    dofAperture = 20.0f,
    dof_max_sigma = 6.0f,
    dof_min_sigma = 0.0001f,

    // diskRadius variables
    diskRadius_k = 1.0f / 25.0f,
    diskRadius_min = 0.0f,

    // shadow opacity: 1.0 - opaque shadow (by default), 0.0 - transparent
    shadowOpacity = 0.65f;

void createProjectionMatrix() {
    projectionMatrix = glm::perspective(glm::radians(90.0f), (float) winWidth / (float) winHeight, 1.0f, 32.0f);
}

void createViewMatrix() {
    viewMatrix = glm::lookAt(cameraPos, cameraLookAt, cameraUp);
}

void updateRenderTextures(const uint &width, const uint &height) {
    cfgtex2D(colorTex, GL_RGB16F, GL_RGB, winWidth, winHeight);
    cfgtex2D(normalTex, GL_RGB16F, GL_RGB, winWidth, winHeight);
    cfgtex2D(ssrValues, GL_RG16F, GL_RG, winWidth, winHeight);
    cfgtex2D(positionTex, GL_RGB16F, GL_RGB, winWidth, winHeight);

    cfgtex2D(screenspaceTex, GL_RGB16F, GL_RGB, winWidth, winHeight);

    cfgtex2D(bloomTex, GL_RGB16F, GL_RGB, winWidth * bloomK, winHeight * bloomK);

    for (size_t i = 0; i < 2; i++) {
        cfgtex2D(pingpongBlurTex[i], GL_RGB16F, GL_RGB, winWidth * bloomK, winHeight * bloomK);
        cfgtex2D(pingpongDofTex[i], GL_RGB16F, GL_RGB, winWidth, winHeight);
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

    updateRenderTextures(width, height);

    createProjectionMatrix();
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

void createShapes(const std::string &path, GLuint params, size_t id, bool inverseNormals = false, GLuint bonesPerVertex = 0) {
    shapes[id].bonesPerVertex = bonesPerVertex;
    shapes[id].init(path, params);
    if (inverseNormals) shapes[id].inverseNormals();
    shapes[id].genBuffers();
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
    window = FULLSCREEN == true ?
        glfwCreateWindow(winWidth, winHeight, "Algine", glfwGetPrimaryMonitor(), nullptr) :
        glfwCreateWindow(winWidth, winHeight, "Algine", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
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
        "src/resources/shaders/ssr/vertex.glsl",
        "src/resources/shaders/ssr/fragment.glsl")
    ));

    params.dofMode = ALGINE_DOF_FROM_COC_MAP;
    std::vector<ShadersData> blus = scompiler::getDOFBlurShader(
        params, dofBlurKernelRadius,
        "src/resources/shaders/dof/vertex.glsl",
        "src/resources/shaders/dof/fragment.glsl"
    );
    dofBlurH.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(
        blus[0]
    ));
    dofBlurV.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(
        blus[1]
    ));

    params.dofMode = ALGINE_CINEMATIC_DOF_MODE_ENABLED;
    dofCoCShader.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getDOFCoCShader(
        params,
        "src/resources/shaders/dof/vertex.glsl",
        "src/resources/shaders/dof/coc_fragment.glsl")
    ));

    blendShader.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getBlendShader(
        params,
        "src/resources/shaders/blend/vertex.glsl",
        "src/resources/shaders/blend/fragment.glsl")
    ));

    bloomSearchShader.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getBloomSearchShader(
        "src/resources/shaders/bloom/vertex_search.glsl",
        "src/resources/shaders/bloom/fragment_search.glsl")
    ));

    std::vector<ShadersData> blur = scompiler::getBlurShader(
        bloomBlurKernelRadius,
        "src/resources/shaders/blur/vertex.glsl",
        "src/resources/shaders/blur/fragment.glsl"
    );
    blurH.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(blur[0]));
    blurV.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(blur[1]));

    CubemapShaderParams cubemapShaderParams;
    cubemapShaderParams.positionOutput = ALGINE_SPHERE_POSITIONS;
    skyboxShader.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getCubemapShader(
        cubemapShaderParams,
        "src/resources/shaders/basic/cubemap_vertex.glsl",
        "src/resources/shaders/basic/cubemap_fragment.glsl")
    ));

    scompiler::loadLocations(cs, params, csp);
    scompiler::loadLocations(ss);
    scompiler::loadLocations(ss_dir);
    scompiler::loadLocations(ssrs);
    scompiler::loadLocations(dofBlurH);
    scompiler::loadLocations(dofBlurV);
    scompiler::loadLocations(dofCoCShader);
    scompiler::loadLocations(blendShader);
    scompiler::loadLocations(bloomSearchShader);
    scompiler::loadLocations(blurH);
    scompiler::loadLocations(blurV);
    scompiler::loadLocations(skyboxShader);

    renderer.ssrs = &ssrs;
    renderer.blendShader = &blendShader;
    renderer.dofBlurShaders[0] = &dofBlurH;
    renderer.dofBlurShaders[1] = &dofBlurV;
    renderer.bloomSearchShader = &bloomSearchShader;
    renderer.blurShaders[0] = &blurH;
    renderer.blurShaders[1] = &blurV;
    renderer.dofCoCShader = &dofCoCShader;
    renderer.prepare();

    skyboxRenderer.init();

    Framebuffer::create(&displayFB);
    Framebuffer::create(&screenspaceFB);
    Framebuffer::create(&bloomSearchFB);
    Framebuffer::create(pingpongFB, 2);
    Framebuffer::create(pingpongBlurFB, 2);
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
    Texture::create(&cocTex);

    TextureCube::create(&skybox);
    TextureCube::CubePaths paths(
        "src/resources/skybox/right.tga", "src/resources/skybox/left.tga",
        "src/resources/skybox/top.tga", "src/resources/skybox/bottom.tga",
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
    applyDefaultTexture2DParams(cocTex);

    updateRenderTextures(winWidth, winHeight);

    GLuint displayColorAttachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    bindFramebuffer(displayFB);
    renderer.configureMainPassRenderbuffer(rbo, winWidth, winHeight);
    glDrawBuffers(4, displayColorAttachments);

    Framebuffer::attachTexture2D(colorTex, COLOR_ATTACHMENT(0));
    Framebuffer::attachTexture2D(normalTex, COLOR_ATTACHMENT(1));
    Framebuffer::attachTexture2D(positionTex, COLOR_ATTACHMENT(2));
    Framebuffer::attachTexture2D(ssrValues, COLOR_ATTACHMENT(3));

    // configuring ping-pong (blur)
    for (size_t i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFB[i]);
        Framebuffer::attachTexture2D(pingpongDofTex[i], COLOR_ATTACHMENT(0));
    }

    // configuring ping-pong (blur)
    for (usize i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongBlurFB[i]);
        Framebuffer::attachTexture2D(pingpongBlurTex[i], COLOR_ATTACHMENT(0));
    }

    // sending to shader center of kernel and right part
    float kernel[bloomBlurKernelRadius * 2 - 1];
    getGB1DKernel(kernel, bloomBlurKernelRadius * 2 - 1, bloomBlurKernelSigma);
    
    for (size_t j = 0; j < 2; j++) {
        glUseProgram(renderer.blurShaders[j]->programId);
        for (uint i = 0; i < bloomBlurKernelRadius; i++) {
            glUniform1f(
                j ? blurV.kernel + (bloomBlurKernelRadius - 1 - i) : blurH.kernel + (bloomBlurKernelRadius - 1 - i),
                kernel[i]
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
    glUniform1f(skyboxShader.positionScaling, 32.0f);

    // blend setting
    glUseProgram(blendShader.programId);
    glUniform1i(blendShader.samplerImage, 0);   // GL_TEXTURE0
    glUniform1i(blendShader.samplerBloom, 1); // GL_TEXTURE1
    glUniform1f(blendShader.exposure, exposure);
    glUniform1f(blendShader.gamma, gamma);

    // blur setting
    for (size_t i = 0; i < 2; i++) {
        glUseProgram(renderer.dofBlurShaders[i]->programId);
        glUniform1i(renderer.dofBlurShaders[i]->samplerImage, 0);
        glUniform1i(renderer.dofBlurShaders[i]->samplerCoCMap, 1);
    }

    // screen space setting
    glUseProgram(ssrs.programId);
    glUniform1i(ssrs.samplerNormalMap, 1);
    glUniform1i(ssrs.samplerSSRValuesMap, 2);
    glUniform1i(ssrs.samplerPositionMap, 3);
    glUseProgram(0);
    useShaderProgram(0);
}

/**
 * Generating Vertex Array Object
 */
void initVAO() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

/**
 * Creating matrices
 */
void initMatrices() {
    createProjectionMatrix();
    createViewMatrix();
}

/**
 * Creating shapes and loading textures
 */
void initShapes() {
    GLuint params = aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices;
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
    glDeleteVertexArrays(1, &vao);
    for (size_t i = 0; i < SHAPES_COUNT; i++) shapes[i].recycle();

    Framebuffer::destroy(&displayFB);
    Framebuffer::destroy(&screenspaceFB);
    Framebuffer::destroy(&bloomSearchFB);
    Framebuffer::destroy(pingpongFB, 2);
    Framebuffer::destroy(pingpongBlurFB, 2);

    Renderbuffer::destroy(&rbo);
}

void sendLampsData() {
	glUniform1i(cs.pointLightsCount, POINT_LAMPS_COUNT); // point lamps count
    glUniform1i(cs.dirLightsCount, DIR_LAMPS_COUNT); // dir lamps count
	setVec3(cs.viewPos, cameraPos); // current camera position
	for (size_t i = 0; i < POINT_LAMPS_COUNT; i++) pointLamps[i].push_pos();
    for (size_t i = 0; i < DIR_LAMPS_COUNT; i++) dirLamps[i].push_pos();
}

/* --- matrices --- */
glm::mat4 getPVMMatrix() {
    return projectionMatrix * viewMatrix * *modelMatrix;
}

glm::mat4 getVMMatrix() {
    return viewMatrix * *modelMatrix;
}

void updateMatrices() {
    setMat4(cs.matPVM, getPVMMatrix());
    setMat4(cs.matVM, getVMMatrix());
    setMat4(cs.matModel, *modelMatrix);
    setMat4(cs.matView, viewMatrix);
}

/**
 * Draws model in depth map
 * @param model
 * @param ss
 */
void drawModelDM(const Model &model, const SShader &ss) {
    if (model.shape->bonesPerVertex != 0) {
        glEnableVertexAttribArray(ss.inBoneWeights);
        glEnableVertexAttribArray(ss.inBoneIds);

        for (size_t i = 0; i < model.shape->bones.size(); i++) {
            setMat4(ss.bones + i, model.shape->bones[i].finalTransformation);
        }
    }

    glUniform1i(ss.boneAttribsPerVertex, model.shape->bonesPerVertex / 4 + (model.shape->bonesPerVertex % 4 == 0 ? 0 : 1));
    for (size_t i = 0; i < model.shape->meshes.size(); i++) {
	    pointer(ss.inPosition, 3, model.shape->meshes[i].getVerticesBuffer());
        if (model.shape->bonesPerVertex != 0) {
            pointer(ss.inBoneWeights, 4, model.shape->meshes[i].getBoneWeightsBuffer());
            pointerui(ss.inBoneIds, 4, model.shape->meshes[i].getBoneIdsBuffer());
        }
	    setMat4(ss.matModel, model.transformation);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.shape->meshes[i].getIndicesBuffer());
        glDrawElements(GL_TRIANGLES, model.shape->meshes[i].indices.size(), GL_UNSIGNED_INT, nullptr);
    }

    if (model.shape->bonesPerVertex != 0) {
        glDisableVertexAttribArray(ss.inBoneWeights);
        glDisableVertexAttribArray(ss.inBoneIds);
    }
}

/**
 * Draws model
 */
void drawModel(Model &model) {
    if (model.shape->bonesPerVertex != 0) {
        glEnableVertexAttribArray(cs.inBoneWeights);
        glEnableVertexAttribArray(cs.inBoneIds);

        for (size_t i = 0; i < model.shape->bones.size(); i++) {
            setMat4(cs.bones + i, model.shape->bones[i].finalTransformation);
        }
    }

    glUniform1i(cs.boneAttribsPerVertex, model.shape->bonesPerVertex / 4 + (model.shape->bonesPerVertex % 4 == 0 ? 0 : 1));
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

	    pointer(cs.inPosition, 3, model.shape->meshes[i].getVerticesBuffer());
	    pointer(cs.inNormal, 3, model.shape->meshes[i].getNormalsBuffer());
	    pointer(cs.inTangent, 3, model.shape->meshes[i].getTangentsBuffer());
	    pointer(cs.inBitangent, 3, model.shape->meshes[i].getBitangentsBuffer());
	    pointer(cs.inTexCoord, 2, model.shape->meshes[i].getTexCoordsBuffer());
        if (model.shape->bonesPerVertex != 0) {
            pointer(cs.inBoneWeights, 4, model.shape->meshes[i].getBoneWeightsBuffer());
            pointerui(cs.inBoneIds, 4, model.shape->meshes[i].getBoneIdsBuffer());
        }
	    modelMatrix = &model.transformation;
	    updateMatrices();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.shape->meshes[i].getIndicesBuffer());
        glDrawElements(GL_TRIANGLES, model.shape->meshes[i].indices.size(), GL_UNSIGNED_INT, nullptr);
    }

    if (model.shape->bonesPerVertex != 0) {
        glDisableVertexAttribArray(cs.inBoneWeights);
        glDisableVertexAttribArray(cs.inBoneIds);
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

	glEnableVertexAttribArray(ss.inPosition);

	// drawing models
	for (size_t i = 0; i < MODELS_COUNT; i++) drawModelDM(models[i], ss);

	// drawing lamps
	for (GLuint i = 0; i < POINT_LAMPS_COUNT; i++) {
		if (i == index) continue;
		drawModelDM(*pointLamps[i].mptr, ss);
	}

	glDisableVertexAttribArray(ss.inPosition);

	pointLamps[index].end();
}

/**
 * Renders to depth map
 */
void renderToDepthMap(uint index) {
	dirLamps[index].begin();
    setMat4(ss_dir.matLightSpace, dirLamps[index].lightSpace);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(ss_dir.inPosition);

	// drawing models
	for (size_t i = 0; i < MODELS_COUNT; i++) drawModelDM(models[i], ss_dir);

	// drawing lamps
	for (GLuint i = 0; i < DIR_LAMPS_COUNT; i++) {
		if (i == index) continue;
		drawModelDM(*dirLamps[i].mptr, ss_dir);
	}

	glDisableVertexAttribArray(ss_dir.inPosition);

	dirLamps[index].end();
}

/**
 * Color rendering
 */
float dof[3] = {32.0f, 0.0f, 0.0f};
uint colorAttachment02[3] = { GL_COLOR_ATTACHMENT0, GL_NONE, GL_COLOR_ATTACHMENT2 };
uint colorAttachment0123[4] = {
    GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3
};
void render() {
    renderer.mainPass(displayFB, rbo);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearBufferfv(GL_COLOR, 1, dof); // dof buffer

	// view port to window size
	glViewport(0, 0, winWidth, winHeight);
	// updating view matrix (because camera position was changed)
	createViewMatrix();

    glDrawBuffers(3, colorAttachment02);
    glDepthFunc(GL_LEQUAL);
    useShaderProgram(skyboxShader.programId);
    setMat4(skyboxShader.matTransform, projectionMatrix * glm::mat4(glm::mat3(viewMatrix)));
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
    skyboxRenderer.render(skyboxShader.inPosition);

    glDrawBuffers(4, colorAttachment0123);
    glDepthFunc(GL_LESS);
    glUseProgram(cs.programId);

	// sending lamps parameters to fragment shader
	sendLampsData();
    glEnableVertexAttribArray(cs.inPosition);
    glEnableVertexAttribArray(cs.inNormal);
    glEnableVertexAttribArray(cs.inTexCoord);
    glEnableVertexAttribArray(cs.inTangent);
    glEnableVertexAttribArray(cs.inBitangent);

    // drawing
    for (size_t i = 0; i < MODELS_COUNT; i++) drawModel(models[i]);
	for (size_t i = 0; i < POINT_LAMPS_COUNT + DIR_LAMPS_COUNT; i++) drawModel(lamps[i]);

    glDisableVertexAttribArray(cs.inPosition);
    glDisableVertexAttribArray(cs.inNormal);
    glDisableVertexAttribArray(cs.inTexCoord);
    glDisableVertexAttribArray(cs.inTangent);
    glDisableVertexAttribArray(cs.inBitangent);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    renderer.screenspacePass(screenspaceFB, colorTex, normalTex, ssrValues, positionTex);

    glViewport(0, 0, winWidth * bloomK, winHeight * bloomK);
    renderer.bloomSearchPass(bloomSearchFB, screenspaceTex);
    renderer.blurPass(pingpongBlurFB, pingpongBlurTex, bloomTex, bloomBlurAmount);
    glViewport(0, 0, winWidth, winHeight);

    renderer.dofCoCPass(cocFB, positionTex);

    renderer.dofBlurPass(pingpongFB, pingpongDofTex, cocTex, screenspaceTex, dofBlurAmount);

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
	glUseProgram(0);
    // dir lights
    glUseProgram(ss_dir.programId);
	for (uint i = 0; i < DIR_LAMPS_COUNT; i++)
        renderToDepthMap(i);
	glUseProgram(0);

    glUseProgram(ssrs.programId);
    setMat4(ssrs.matProjection, projectionMatrix);
    setMat4(ssrs.matView, viewMatrix);
    // glUseProgram(0); - not need

	/* --- color rendering --- */
	glUseProgram(cs.programId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    initVAO();
    initMatrices();
    initShapes();
    createModels();
    initLamps();
    initShadowMaps();
    initShadowCalculation();
    initDOF();

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

    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);

    else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        for (int i = 0; i < 6; i++) {
            GLfloat *pixels = getTexImageCube(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pointLamps[0].depthMap, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, GL_DEPTH_COMPONENT);
            saveTexImage(pixels, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 1, io::getCurrentDir() + "/out/scr_" + std::to_string(i) + ".bmp", 3, false);
            delete[] pixels;
        }
        GLfloat *pixels = getTexImage2D(dirLamps[0].depthMap, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, GL_DEPTH_COMPONENT);
        saveTexImage(pixels, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 1, io::getCurrentDir() + "/out/dir_depth.bmp", 3);
        delete[] pixels;
        std::cout << "Depth map data saved\n";

        pixels = getTexImage2D(bloomTex, winWidth * bloomK, winHeight * bloomK, GL_RGB);
        saveTexImage(pixels, winWidth * bloomK, winHeight * bloomK, 3, io::getCurrentDir() + "/out/scr_screenspace.bmp", 3);
        delete[] pixels;

        pixels = getTexImage2D(pingpongBlurTex[!renderer.horizontal], winWidth * bloomK, winHeight * bloomK, GL_RGB);
        saveTexImage(pixels, winWidth * bloomK, winHeight * bloomK, 3, io::getCurrentDir() + "/out/scr_screenspace_blur.bmp", 3);
        delete[] pixels;

        pixels = getTexImage2D(screenspaceTex, winWidth, winHeight, GL_RGB);
        saveTexImage(pixels, winWidth, winHeight, 3, io::getCurrentDir() + "/out/scr_screenspace_.bmp", 3);
        delete[] pixels;

        std::cout << "Screenspace map data saved\n";
    }
}

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x;
        double y;
        glfwGetCursorPos(window, &x, &y);
        std::cout << "x: " << x << "; y: " << y << "\n";

        GLfloat *pixels = getPixels(positionTex, (size_t) x, winHeight - (size_t) y, 1, 1, GL_RGB);
        
        std::cout << "x: " << pixels[0] << "; y: " << pixels[1] << "; z: " << pixels[2] << "\n";
        
        useShaderProgram(dofCoCShader.programId);
        glUniform1f(dofCoCShader.cinematicDOFPlaneInFocus, pixels[2] == 0 ? FLT_EPSILON : pixels[2]);
        useShaderProgram(0);
        
        delete[] pixels;
    }
}