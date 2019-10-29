/**
 * Algine: C++ OpenGL Engine
 *
 * My telegram: https://t.me/congard
 * My github: https://github.com/congard
 * @author congard
 */

// #define debug_sm

#define GLM_FORCE_CTOR_INIT

#include <iostream>
#include <thread>
#include <chrono>

#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>

// TODO: Something to do with ALGINE_LOGGING...
//  This macro appears in some constructors / operators / destructors.
//  Maybe simply remove?
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

#define pointLampsCount 1u
#define dirLampsCount 1u
#define pointLightsLimit 8u
#define dirLightsLimit 8u
#define maxBoneAttribsPerVertex 1u
#define maxBones 64u
// point light texture start id
#define POINT_LIGHT_TSID 6
// dir light texture start id
#define DIR_LIGHT_TSID POINT_LIGHT_TSID + pointLampsCount
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
const glm::mat4 *modelMatrix; // model matrix stored in Model::transformation

// framebuffers, textures etc for rendering
using namespace algine;

// models
Shape shapes[SHAPES_COUNT];
Model models[MODELS_COUNT], lamps[pointLampsCount + dirLampsCount];
Animator manAnimator, astroboyAnimator; // animator for man, astroboy models

// light
PointLamp pointLamps[pointLampsCount];
DirLamp dirLamps[dirLampsCount];
LightDataSetter lightDataSetter;

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

ShaderProgram *skyboxShader;
ShaderProgram *colorShader;
ShaderProgram *pointShadowShader;
ShaderProgram *dirShadowShader;
ShaderProgram *dofBlurHorShader, *dofBlurVertShader;
ShaderProgram *dofCoCShader;
ShaderProgram *ssrShader;
ShaderProgram *bloomSearchShader;
ShaderProgram *bloomBlurHorShader, *bloomBlurVertShader;
ShaderProgram *cocBlurHorShader, *cocBlurVertShader;
ShaderProgram *blendShader;
ShaderProgram *blurBloomShaders[2];
ShaderProgram *blurCoCShaders[2];

MouseEventListener mouseEventListener;

Camera camera;
FPSCameraController camController;

EulerRotator manHeadRotator;

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

    result.initShadowMapping(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

    colorShader->use();
    lightDataSetter.setColor(result, id);
    lightDataSetter.setKc(result, id);
    lightDataSetter.setKl(result, id);
    lightDataSetter.setKq(result, id);
    lightDataSetter.setFarPlane(result, id);
    lightDataSetter.setBias(result, id);
    ShaderProgram::reset();
}

void createDirLamp(DirLamp &result, const glm::vec3 &pos, const glm::vec3 &color, usize id) {
    result.pos = pos;
    result.color = color;
    result.kc = 1;
    result.kl = 0.045f;
    result.kq = 0.0075f;

    result.orthoShadowMapping(-10.0f, 10.0f, -10.0f, 10.0f);
    result.initShadowMapping(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

    colorShader->use();
    lightDataSetter.setColor(result, id);
    lightDataSetter.setKc(result, id);
    lightDataSetter.setKl(result, id);
    lightDataSetter.setKq(result, id);
    lightDataSetter.setMinBias(result, id);
    lightDataSetter.setMaxBias(result, id);
    lightDataSetter.setLightMatrix(result, id);
    ShaderProgram::reset();
}

void createShapes(const std::string &path, const uint params, const size_t id, const bool inverseNormals = false, const uint bonesPerVertex = 0) {
    shapes[id].bonesPerVertex = bonesPerVertex;
    shapes[id].init(path, params);
    if (inverseNormals) shapes[id].inverseNormals();
    shapes[id].genBuffers();
    shapes[id].createVAO(
            pointShadowShader->getLocation(AlgineNames::ShadowShader::InPos),
            -1, -1, -1, -1,
            pointShadowShader->getLocation(AlgineNames::ShadowShader::InBoneWeights),
            pointShadowShader->getLocation(AlgineNames::ShadowShader::InBoneIds)
    ); // all shadow shaders have same ids
    shapes[id].createVAO(
                colorShader->getLocation(AlgineNames::ColorShader::InPos),
                colorShader->getLocation(AlgineNames::ColorShader::InTexCoord),
                colorShader->getLocation(AlgineNames::ColorShader::InNormal),
                colorShader->getLocation(AlgineNames::ColorShader::InTangent),
                colorShader->getLocation(AlgineNames::ColorShader::InBitangent),
                colorShader->getLocation(AlgineNames::ColorShader::InBoneWeights),
                colorShader->getLocation(AlgineNames::ColorShader::InBoneIds)
    );
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
    ShaderProgram::create(skyboxShader, colorShader, pointShadowShader, dirShadowShader,
                          dofBlurHorShader, dofBlurVertShader, dofCoCShader, ssrShader,
                          bloomSearchShader, bloomBlurHorShader, bloomBlurVertShader,
                          cocBlurHorShader, cocBlurVertShader, blendShader);

    blurBloomShaders[0] = bloomBlurHorShader;
    blurBloomShaders[1] = bloomBlurVertShader;
    blurCoCShaders[0] = cocBlurHorShader;
    blurCoCShaders[1] = cocBlurVertShader;

    std::cout << "Compiling algine shaders\n";

    {
        using namespace AlgineNames::ShaderDefinitions;

        ShaderManager manager;

        // color shader
        manager.fromFile("src/resources/shaders/vertex_shader.glsl",
                         "src/resources/shaders/fragment_shader.glsl");
        manager.define(Lighting::Lighting);
        manager.define(Lighting::LightingAttenuation);
        manager.define(Lighting::NormalMapping);
        manager.define(Lighting::ShadowMappingPCF);
        manager.define(TextureMapping);
        manager.define(BoneSystem);
        manager.define(SSR);
        manager.define(Lighting::PointLightsLimit, std::to_string(pointLightsLimit));
        manager.define(Lighting::DirLightsLimit, std::to_string(dirLightsLimit));
        manager.define(MaxBoneAttribsPerVertex, std::to_string(maxBoneAttribsPerVertex));
        manager.define(MaxBones, std::to_string(maxBones));
        colorShader->fromSource(manager.makeGenerated());
        colorShader->loadActiveLocations();

        // point shadow shader
        manager.fromFile("src/resources/shaders/shadow/vertex_shadow_shader.glsl",
                         "src/resources/shaders/shadow/fragment_shadow_shader.glsl",
                         "src/resources/shaders/shadow/geometry_shadow_shader.glsl");
        manager.resetDefinitions();
        manager.define(BoneSystem);
        manager.define(MaxBoneAttribsPerVertex, std::to_string(maxBoneAttribsPerVertex));
        manager.define(MaxBones, std::to_string(maxBones));
        manager.define(ShadowShader::PointLightShadowMapping);
        pointShadowShader->fromSource(manager.makeGenerated());
        pointShadowShader->loadActiveLocations();

        // dir shadow shader
        ShadersData shadowShaderTemplate = manager.getTemplate();
        shadowShaderTemplate.geometry = std::string(); // we don't need geometry shader for dir light shadows
        manager.fromSource(shadowShaderTemplate);
        manager.removeDefinition(ShadowShader::PointLightShadowMapping);
        manager.define(ShadowShader::DirLightShadowMapping);
        dirShadowShader->fromSource(manager.makeGenerated());
        dirShadowShader->loadActiveLocations();

        // SSR shader
        ssrShader->fromFile("src/resources/shaders/basic/quad_vertex.glsl",
                            "src/resources/shaders/ssr/fragment.glsl");
        ssrShader->loadActiveLocations();

        // bloom search shader
        bloomSearchShader->fromFile("src/resources/shaders/basic/quad_vertex.glsl",
                                    "src/resources/shaders/bloom/fragment_search.glsl");
        bloomSearchShader->loadActiveLocations();

        // TODO: remove "dof blur shaders", replace with only blur + blend steps
        // DOF blur shaders
        manager.fromFile("src/resources/shaders/basic/quad_vertex.glsl",
                         "src/resources/shaders/dof/fragment.glsl");
        manager.resetDefinitions();
        manager.define(Dof::DofCocMap);
        manager.define(Blur::KernelRadius, std::to_string(dofBlurKernelRadius));
        manager.define(Dof::BleedingMinDeltaZ);
        manager.define(Dof::BleedingMinDeltaCoC);
        manager.define(Dof::BleedingMaxFocusCoC);
        manager.define(Blur::Horizontal);
        dofBlurHorShader->fromSource(manager.makeGenerated());
        dofBlurHorShader->loadActiveLocations();

        manager.resetGenerated();
        manager.removeDefinition(Blur::Horizontal);
        manager.define(Blur::Vertical);
        dofBlurVertShader->fromSource(manager.makeGenerated());
        dofBlurVertShader->loadActiveLocations();

        // DOF CoC shader
        manager.fromFile("src/resources/shaders/basic/quad_vertex.glsl",
                         "src/resources/shaders/dof/coc_fragment.glsl");
        manager.resetDefinitions();
        manager.define(Dof::CinematicDof);
        dofCoCShader->fromSource(manager.makeGenerated());
        dofCoCShader->loadActiveLocations();

        // blend shader
        manager.fromFile("src/resources/shaders/basic/quad_vertex.glsl",
                         "src/resources/shaders/blend/fragment.glsl");
        manager.resetDefinitions();
        manager.define(Bloom::BloomAdd);
        blendShader->fromSource(manager.makeGenerated());
        blendShader->loadActiveLocations();

        // bloom blur shaders
        manager.fromFile("src/resources/shaders/basic/quad_vertex.glsl",
                         "src/resources/shaders/blur/fragment.glsl");
        manager.resetDefinitions();
        manager.define(Blur::KernelRadius, std::to_string(bloomBlurKernelRadius));
        manager.define(OutputType, "vec3");
        manager.define(TexComponent, "rgb");
        manager.define(Blur::Horizontal);
        bloomBlurHorShader->fromSource(manager.makeGenerated());
        bloomBlurHorShader->loadActiveLocations();

        manager.resetGenerated();
        manager.removeDefinition(Blur::Horizontal);
        manager.define(Blur::Vertical);
        bloomBlurVertShader->fromSource(manager.makeGenerated());
        bloomBlurVertShader->loadActiveLocations();

        // CoC blur shaders
        manager.resetGenerated();
        manager.resetDefinitions();
        manager.define(Blur::KernelRadius, std::to_string(cocBlurKernelRadius));
        manager.define(OutputType, "float");
        manager.define(TexComponent, "r");
        manager.define(Blur::Horizontal);
        cocBlurHorShader->fromSource(manager.makeGenerated());
        cocBlurHorShader->loadActiveLocations();

        manager.resetGenerated();
        manager.removeDefinition(Blur::Horizontal);
        manager.define(Blur::Vertical);
        cocBlurVertShader->fromSource(manager.makeGenerated());
        cocBlurVertShader->loadActiveLocations();

        // cubemap shader
        manager.fromFile("src/resources/shaders/basic/cubemap_vertex.glsl",
                         "src/resources/shaders/basic/cubemap_fragment.glsl");
        manager.resetDefinitions();
        manager.define(CubemapShader::SpherePositions);
        manager.define(CubemapShader::ColorOut, "0"); // TODO: create constants
        manager.define(CubemapShader::PosOut, "2");
        manager.define(OutputType, "vec3");
        skyboxShader->fromSource(manager.makeGenerated());
        skyboxShader->loadActiveLocations();
    }

    std::cout << "Compilation done\n";

    #define value *

    lightDataSetter.indexDirLightLocations(colorShader, dirLightsLimit);
    lightDataSetter.indexPointLightLocations(colorShader, pointShadowShader, pointLightsLimit);

    renderer.ssrShader = ssrShader;
    renderer.blendShader = blendShader;
    renderer.dofBlurShaders[0] = dofBlurHorShader;
    renderer.dofBlurShaders[1] = dofBlurVertShader;
    renderer.bloomSearchShader = bloomSearchShader;
    renderer.blurShaders = blurBloomShaders;
    renderer.dofCoCShader = dofCoCShader;
    renderer.quadRenderer = &quadRenderer;

    skyboxRenderer.init(skyboxShader->getLocation(AlgineNames::CubemapShader::InPos));
    quadRenderer.init(0, 1); // inPosLocation in quad shader is 0, inTexCoordLocation is 1

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
        blurBloomShaders[j]->use();
        for (int i = 0; i < bloomBlurKernelRadius; i++)
            ShaderProgram::set(blurBloomShaders[j]->getLocation(AlgineNames::BlurShader::Kernel) + (bloomBlurKernelRadius - 1 - i), kernelBloom[i]);

        blurCoCShaders[j]->use();
        for (int i = 0; i < cocBlurKernelRadius; i++)
            ShaderProgram::set(blurCoCShaders[j]->getLocation(AlgineNames::BlurShader::Kernel) + (cocBlurKernelRadius - 1 - i), kernelCoC[i]);
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
    colorShader->use();
    colorShader->set(AlgineNames::ColorShader::Material::AmbientTex, 0);
    colorShader->set(AlgineNames::ColorShader::Material::DiffuseTex, 1);
    colorShader->set(AlgineNames::ColorShader::Material::SpecularTex, 2);
    colorShader->set(AlgineNames::ColorShader::Material::NormalTex, 3);
    colorShader->set(AlgineNames::ColorShader::Material::ReflectionStrengthTex, 4);
    colorShader->set(AlgineNames::ColorShader::Material::JitterTex, 5);
    colorShader->set(AlgineNames::ColorShader::ShadowOpacity, shadowOpacity);

    // configuring CubemapShader
    skyboxShader->use();
    skyboxShader->set(AlgineNames::CubemapShader::Color, glm::vec3(0.125f));
    skyboxShader->set(AlgineNames::CubemapShader::PosScaling, 64.0f);

    // blend setting
    blendShader->use();
    blendShader->set(AlgineNames::BlendBloomShader::BaseImage, 0); // GL_TEXTURE0
    blendShader->set(AlgineNames::BlendBloomShader::BloomImage, 1); // GL_TEXTURE1
    blendShader->set(AlgineNames::BlendBloomShader::Exposure, blendExposure);
    blendShader->set(AlgineNames::BlendBloomShader::Gamma, blendGamma);

    // dof blur setting
    for (size_t i = 0; i < 2; i++) {
        renderer.dofBlurShaders[i]->use();
        renderer.dofBlurShaders[i]->set(AlgineNames::DOFShader::BaseImage, 0);
        renderer.dofBlurShaders[i]->set(AlgineNames::DOFShader::CoCMap, 1);
        renderer.dofBlurShaders[i]->set(AlgineNames::DOFShader::PositionMap, 2);
        renderer.dofBlurShaders[i]->set(AlgineNames::DOFShader::BleedingMinDeltaZ, bleedingEliminationMinDeltaZ);
        renderer.dofBlurShaders[i]->set(AlgineNames::DOFShader::BleedingMinDeltaCoC, bleedingEliminationMinDeltaCoC);
        renderer.dofBlurShaders[i]->set(AlgineNames::DOFShader::BleedingMaxFocusCoC, bleedingEliminationMaxFocusCoC);
    }

    // screen space setting
    ssrShader->use();
    ssrShader->set(AlgineNames::SSRShader::NormalMap, 1);
    ssrShader->set(AlgineNames::SSRShader::SSRValuesMap, 2);
    ssrShader->set(AlgineNames::SSRShader::PositionMap, 3);
    glUseProgram(0);
}

/**
 * Creating matrices
 */
void initCamera() {
    camera.far = 64.0f;
    camera.fov = glm::radians(60.0f);
    camera.aspectRatio = (float)winWidth / (float)winHeight;
    camera.perspective();
    
    camera.setPitch(glm::radians(30.0f));
    camera.rotate();

    camera.setPos(0, 10, 14);
    camera.translate();

    camera.updateMatrix();
    
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
    models[0] = Model(Rotator::RotatorTypeSimple);
    models[0].shape = &shapes[0];

    // animated man
    manAnimator = Animator(AnimShape(&shapes[2].animations, &shapes[2].bones, &shapes[2].globalInverseTransform, &shapes[2].rootNode));
    models[1] = Model(Rotator::RotatorTypeSimple);
    models[1].shape = &shapes[2];
    models[1].setPitch(glm::radians(90.0f));
    models[1].rotate();
    models[1].setX(-2.0f);
    models[1].translate();
    models[1].updateMatrix();
    models[1].animator = &manAnimator;

    // animated astroboy
    astroboyAnimator = Animator(AnimShape(&shapes[3].animations, &shapes[3].bones, &shapes[3].globalInverseTransform, &shapes[3].rootNode));
    models[2] = Model(Rotator::RotatorTypeSimple);
    models[2].shape = &shapes[3];
    models[2].setPitch(glm::radians(90.0f));
    models[2].rotate();
    models[2].setScale(glm::vec3(50.0f));
    models[2].scale();
    models[2].setX(2.0f);
    models[2].translate();
    models[2].updateMatrix();
    models[2].animator = &astroboyAnimator;
}

/**
 * Creating light sources
 */
void initLamps() {
    createPointLamp(pointLamps[0], glm::vec3(0.0f, 8.0f, 15.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0);
    lamps[0] = Model(Rotator::RotatorTypeSimple);
    lamps[0].shape = &shapes[1];
    pointLamps[0].mptr = &lamps[0];
    lamps[0].setPos(pointLamps[0].pos);
    lamps[0].translate();
    lamps[0].updateMatrix();

    createDirLamp(dirLamps[0], glm::vec3(0.0f, 8.0f, -15.0f), glm::vec3(253.0f / 255.0f, 184.0f / 255.0f, 19.0f / 255.0f), 0);
    lamps[1] = Model(Rotator::RotatorTypeSimple);
    lamps[1].shape = &shapes[1];
    dirLamps[0].mptr = &lamps[1];
    lamps[1].setPos(dirLamps[0].pos);
    lamps[1].translate();
    lamps[1].updateMatrix();
}

/**
 * Binds to depth cubemaps
 */
void initShadowMaps() {
    colorShader->use();
    // to avoid black screen on AMD GPUs and old Intel HD Graphics
    // TODO: maybe move it to LightDataSetter?
    for (int i = 0; i < pointLightsLimit; i++) {
        ShaderProgram::set(lightDataSetter.getLocation(LightDataSetter::ShadowMap, Light::TypePointLight, i), POINT_LIGHT_TSID + i);
		glActiveTexture(GL_TEXTURE0 + POINT_LIGHT_TSID + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    for (usize i = 0; i < pointLampsCount; i++)
        lightDataSetter.setShadowMap(pointLamps[i], i, POINT_LIGHT_TSID + i);

    // to avoid black screen on AMD GPUs and old Intel HD Graphics
    for (int i = 0; i < dirLightsLimit; i++) {
        ShaderProgram::set(lightDataSetter.getLocation(LightDataSetter::ShadowMap, Light::TypeDirLight, i),
                           static_cast<int>(DIR_LIGHT_TSID) + i); // Mesa drivers require int as sampler, not uint
		glActiveTexture(GL_TEXTURE0 + DIR_LIGHT_TSID + i);
		glBindTexture(GL_TEXTURE_2D, 0);
    }
    for (usize i = 0; i < dirLampsCount; i++)
        lightDataSetter.setShadowMap(dirLamps[i], i, DIR_LIGHT_TSID + i);
    glUseProgram(0);
}

void initShadowCalculation() {
    colorShader->use();
    colorShader->set(AlgineNames::ColorShader::ShadowDiskRadiusK, diskRadius_k);
    colorShader->set(AlgineNames::ColorShader::ShadowDiskRadiusMin, diskRadius_min);
    glUseProgram(0);
}

/**
 * Initialize Depth of field
 */
void initDOF() {
    dofCoCShader->use();
    //dofCoCShader->set(ALGINE_DOF_SIGMA_MAX, dof_max_sigma);
    //dofCoCShader->set(ALGINE_DOF_SIGMA_MIN, dof_min_sigma);
    dofCoCShader->set(AlgineNames::DOFShader::Aperture, dofAperture);
    dofCoCShader->set(AlgineNames::DOFShader::ImageDistance, dofImageDistance);
    dofCoCShader->set(AlgineNames::DOFShader::PlaneInFocus, -1.0f);
    glUseProgram(0);
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
    lightDataSetter.setPointLightsCount(pointLampsCount);
    lightDataSetter.setDirLightsCount(dirLampsCount);
    colorShader->set(AlgineNames::ColorShader::CameraPos, camera.getPosition());
    for (size_t i = 0; i < pointLampsCount; i++)
        lightDataSetter.setPos(pointLamps[i], i);
    for (size_t i = 0; i < dirLampsCount; i++)
        lightDataSetter.setPos(dirLamps[i], i);
}

/* --- matrices --- */
glm::mat4 getMVPMatrix() {
    return camera.getProjectionMatrix() * camera.getViewMatrix() * value modelMatrix;
}

glm::mat4 getMVMatrix() {
    return camera.getViewMatrix() * value modelMatrix;
}

void updateMatrices() {
    colorShader->set(AlgineNames::ColorShader::MVPMatrix, getMVPMatrix());
    colorShader->set(AlgineNames::ColorShader::MVMatrix, getMVMatrix());
    colorShader->set(AlgineNames::ColorShader::ModelMatrix, value modelMatrix);
    colorShader->set(AlgineNames::ColorShader::ViewMatrix, camera.getViewMatrix());
}

/**
 * Draws model in depth map<br>
 * if point light, leave mat empty, but if dir light - it must be light space matrix
 */
void drawModelDM(const Model &model, ShaderProgram *program, const glm::mat4 &mat = glm::mat4(1.0f)) {
    glBindVertexArray(model.shape->vaos[0]);

    if (model.shape->bonesPerVertex != 0) {
        for (int i = 0; i < model.shape->bones.size(); i++) {
            program->set(program->getLocation(AlgineNames::ShadowShader::Bones) + i, model.shape->bones[i].finalTransformation);
        }
    }

    program->set(AlgineNames::ShadowShader::BoneAttribsPerVertex, (int)(model.shape->bonesPerVertex / 4 + (model.shape->bonesPerVertex % 4 == 0 ? 0 : 1)));
    program->set(AlgineNames::ShadowShader::TransformationMatrix, mat * model.m_transform);
    
    for (size_t i = 0; i < model.shape->meshes.size(); i++) {
        glDrawElements(GL_TRIANGLES, model.shape->meshes[i].count, GL_UNSIGNED_INT, reinterpret_cast<void*>(model.shape->meshes[i].start * sizeof(uint)));
    }
}

/**
 * Draws model
 */
void drawModel(const Model &model) {
    glBindVertexArray(model.shape->vaos[1]);
    
    if (model.shape->bonesPerVertex != 0) {
        for (int i = 0; i < model.shape->bones.size(); i++) {
            colorShader->set(colorShader->getLocation(AlgineNames::ColorShader::Bones) + i, model.shape->bones[i].finalTransformation);
        }
    }

    colorShader->set(AlgineNames::ColorShader::BoneAttribsPerVertex, (int)(model.shape->bonesPerVertex / 4 + (model.shape->bonesPerVertex % 4 == 0 ? 0 : 1)));
    modelMatrix = &model.m_transform;
	updateMatrices();
    for (size_t i = 0; i < model.shape->meshes.size(); i++) {
        texture2DAB(0, model.shape->meshes[i].mat.ambientTexture);
        texture2DAB(1, model.shape->meshes[i].mat.diffuseTexture);
        texture2DAB(2, model.shape->meshes[i].mat.specularTexture);
        texture2DAB(3, model.shape->meshes[i].mat.normalTexture);
        texture2DAB(4, model.shape->meshes[i].mat.reflectionTexture);
        texture2DAB(5, model.shape->meshes[i].mat.jitterTexture);

        colorShader->set(AlgineNames::ColorShader::Material::AmbientStrength, model.shape->meshes[i].mat.ambientStrength);
        colorShader->set(AlgineNames::ColorShader::Material::DiffuseStrength, model.shape->meshes[i].mat.diffuseStrength);
        colorShader->set(AlgineNames::ColorShader::Material::SpecularStrength, model.shape->meshes[i].mat.specularStrength);
        colorShader->set(AlgineNames::ColorShader::Material::Shininess, model.shape->meshes[i].mat.shininess);

        glDrawElements(GL_TRIANGLES, model.shape->meshes[i].count, GL_UNSIGNED_INT, reinterpret_cast<void*>(model.shape->meshes[i].start * sizeof(uint)));
    }
}

/**
 * Renders to depth cubemap
 */
void renderToDepthCubemap(const uint index) {
	pointLamps[index].begin();
	pointLamps[index].updateMatrices();
    lightDataSetter.setShadowShaderPos(pointLamps[index]);
	lightDataSetter.setShadowShaderMatrices(pointLamps[index]);
	glClear(GL_DEPTH_BUFFER_BIT);

	// drawing models
    for (size_t i = 0; i < MODELS_COUNT; i++)
        drawModelDM(models[i], pointShadowShader);

	// drawing lamps
	for (GLuint i = 0; i < pointLampsCount; i++) {
		if (i == index) continue;
        drawModelDM(*pointLamps[i].mptr, pointShadowShader);
	}

	pointLamps[index].end();
}

/**
 * Renders to depth map
 */
void renderToDepthMap(uint index) {
	dirLamps[index].begin();
	glClear(GL_DEPTH_BUFFER_BIT);

	// drawing models
    for (size_t i = 0; i < MODELS_COUNT; i++)
        drawModelDM(models[i], dirShadowShader, dirLamps[index].lightSpace);

	// drawing lamps
	for (GLuint i = 0; i < dirLampsCount; i++) {
		if (i == index) continue;
        drawModelDM(*dirLamps[i].mptr, dirShadowShader, dirLamps[index].lightSpace);
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
    renderer.mainPass(displayFB);
    
	// view port to window size
	glViewport(0, 0, winWidth, winHeight);

    glDrawBuffers(4, colorAttachment0123);
    colorShader->use();

    // sending lamps parameters to fragment shader
	sendLampsData();

    // drawing
    for (size_t i = 0; i < MODELS_COUNT; i++)
        drawModel(models[i]);
	for (size_t i = 0; i < pointLampsCount + dirLampsCount; i++)
	    drawModel(lamps[i]);

    // render skybox
    glDepthFunc(GL_LEQUAL);
    glDrawBuffers(3, colorAttachment02);
    skyboxShader->use();
    skyboxShader->set(AlgineNames::CubemapShader::ViewMatrix, glm::mat3(camera.getViewMatrix()));
    skyboxShader->set(AlgineNames::CubemapShader::TransformationMatrix, camera.getProjectionMatrix() * glm::mat4(glm::mat3(camera.getViewMatrix())));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
    skyboxRenderer.render();
    glDepthFunc(GL_LESS);
    glDrawBuffers(4, colorAttachment0123);

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

    // shadow rendering
    // point lights
    pointShadowShader->use();
	for (uint i = 0; i < pointLampsCount; i++) {
	    lightDataSetter.setShadowShaderFarPlane(pointLamps[i]);
        renderToDepthCubemap(i);
    }

    // dir lights
    dirShadowShader->use();
    for (uint i = 0; i < dirLampsCount; i++)
        renderToDepthMap(i);
	
    ssrShader->use();
    ssrShader->set(AlgineNames::SSRShader::ProjectionMatrix, camera.getProjectionMatrix());
    ssrShader->set(AlgineNames::SSRShader::ViewMatrix, camera.getViewMatrix());

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
        pointLamps[0].mptr->updateMatrix();
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
    else if (action == GLFW_REPEAT || action == GLFW_RELEASE) {
        if (key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D) {
            if (key == GLFW_KEY_W)
                camController.goForward();
            else if (key == GLFW_KEY_S)
                camController.goBack();
            else if (key == GLFW_KEY_A)
                camController.goLeft();
            else if (key == GLFW_KEY_D)
                camController.goRight();

            camera.translate();
            camera.updateMatrix();
        } else if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT || key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
            glm::mat4 r;
            if (key == GLFW_KEY_LEFT)
                manHeadRotator.setYaw(manHeadRotator.getYaw() + glm::radians(5.0f));
            else if (key == GLFW_KEY_RIGHT)
                manHeadRotator.setYaw(manHeadRotator.getYaw() - glm::radians(5.0f));
            else if (key == GLFW_KEY_UP)
                manHeadRotator.setPitch(manHeadRotator.getPitch() + glm::radians(5.0f));
            else if (key == GLFW_KEY_DOWN)
                manHeadRotator.setPitch(manHeadRotator.getPitch() - glm::radians(5.0f));

            manHeadRotator.rotate(r);
            shapes[2].setNodeTransform("Armature_Head", r);
        }
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
            camera.rotate();
            camera.updateMatrix();
            break;
        case MouseEventListener::ActionLongClick:
            std::cout << "Long click " << event->button << "\n";
            break;
        case MouseEventListener::ActionClick:
            std::cout << "x: " << event->getX() << "; y: " << event->getY() << "\n";
        
            GLfloat *pixels = getPixels(positionTex, (size_t)event->getX(), winHeight - (size_t)event->getY(), 1, 1, GL_RGB);
        
            std::cout << "x: " << pixels[0] << "; y: " << pixels[1] << "; z: " << pixels[2] << "\n";
        
            dofCoCShader->use();
            dofCoCShader->set(AlgineNames::DOFShader::PlaneInFocus, pixels[2] == 0 ? FLT_EPSILON : pixels[2]);
            glUseProgram(0);
        
            delete[] pixels;
            break;
    }
}
