/**
 * Algine: C++ OpenGL Engine
 *
 * My telegram: https://t.me/congard
 * My github: https://github.com/congard
 * @author congard
 */

#define GLM_FORCE_CTOR_INIT

#include <iostream>
#include <thread>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <tulz/Path>

#include <algine/algine_renderer.h>
#include <algine/framebuffer.h>
#include <algine/light.h>
#include <algine/constants/BoneSystem.h>
#include <algine/constants/ColorShader.h>
#include <algine/constants/ShadowShader.h>
#include <algine/constants/CubemapShader.h>
#include <algine/ext/constants/SSRShader.h>
#include <algine/ext/constants/BlendBloomModule.h>
#include <algine/ext/constants/DOFShaders.h>
#include <algine/camera.h>
#include <algine/renderbuffer.h>
#include <algine/debug.h>
#include <algine/gputils.h>
#include <algine/event.h>
#include <algine/shader.h>
#include <algine/texture.h>
#include <algine/CubeRenderer.h>
#include <algine/QuadRenderer.h>
#include <algine/ext/Blur.h>
#include <algine/ext/constants/BlurShader.h>

#define SHADOW_MAP_RESOLUTION 1024
#define bloomK 0.5f
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
#define DIR_LIGHT_TSID (int)(POINT_LIGHT_TSID + pointLightsLimit)
#define SHAPES_COUNT 4
#define MODELS_COUNT 3

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(algine::MouseEventListener::MouseEvent *event);
void cursor_pos_callback(GLFWwindow* window, double x, double y);

// framebuffers, textures etc for rendering
using namespace algine;
using namespace std;
using namespace tulz;

// Window dimensions
uint winWidth = 1366, winHeight = 763;
GLFWwindow* window;

// matrices
const glm::mat4 *modelMatrix; // model matrix stored in Model::transformation

// models
shared_ptr<Shape> shapes[SHAPES_COUNT];
Model models[MODELS_COUNT], lamps[pointLampsCount + dirLampsCount];
Animator manAnimator, astroboyAnimator; // animator for man, astroboy models

// light
PointLamp pointLamps[pointLampsCount];
DirLamp dirLamps[dirLampsCount];
LightDataSetter lightDataSetter;

// renderer
AlgineRenderer renderer;
shared_ptr<CubeRenderer> skyboxRenderer;
shared_ptr<QuadRenderer> quadRenderer;

shared_ptr<Blur> bloomBlur;
shared_ptr<Blur> cocBlur;

Renderbuffer *rbo;
Framebuffer *displayFb;
Framebuffer *screenspaceFb;
Framebuffer *bloomSearchFb;
Framebuffer *pingpongFb[2];
Framebuffer *cocFb;

Texture2D *colorTex;
Texture2D *normalTex;
Texture2D *ssrValues;
Texture2D *positionTex;
Texture2D *screenspaceTex;
Texture2D *bloomTex;
Texture2D *pingpongDofTex[2];
Texture2D *cocTex;
TextureCube *skybox;

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

inline void updateTexture(Texture *const texture, const uint width, const uint height) {
    texture->setWidthHeight(width, height);
    texture->bind();
    texture->update();
}

void updateRenderTextures() {
    updateTexture(colorTex, winWidth, winHeight);
    updateTexture(normalTex, winWidth, winHeight);
    updateTexture(ssrValues, winWidth, winHeight);
    updateTexture(positionTex, winWidth, winHeight);
    updateTexture(screenspaceTex, winWidth, winHeight); // TODO: make small sst + blend pass in the future
    updateTexture(bloomTex, winWidth * bloomK, winHeight * bloomK);

    for (size_t i = 0; i < 2; ++i) {
        updateTexture(bloomBlur->getPingPongTextures()[i], winWidth * bloomK, winHeight * bloomK);
        updateTexture(pingpongDofTex[i], winWidth, winHeight);
        updateTexture(cocBlur->getPingPongTextures()[i], winWidth, winHeight);
    }

    updateTexture(cocTex, winWidth, winHeight);
}

/**
 * To correctly display the scene when changing the window size
 */
void window_size_callback(GLFWwindow* window, int width, int height) {
    winWidth = width;
    winHeight = height;

    rbo->bind();
    rbo->setWidthHeight(width, height);
    rbo->update();
    rbo->unbind();

    updateRenderTextures();

    camera.setAspectRatio((float)winWidth / (float)winHeight);
    camera.perspective();
}

/**
 * Creates Lamp with default params
 */
void createPointLamp(PointLamp &result, const glm::vec3 &pos, const glm::vec3 &color, usize id) {
    result.setPos(pos);
    result.translate();
    result.setColor(color);
    result.setKc(1.0f);
    result.setKl(0.045f);
    result.setKq(0.0075f);

    result.perspectiveShadows();
    result.updateMatrix();
    result.initShadows(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

    colorShader->use();
    lightDataSetter.setColor(result, id);
    lightDataSetter.setKc(result, id);
    lightDataSetter.setKl(result, id);
    lightDataSetter.setKq(result, id);
    lightDataSetter.setFarPlane(result, id);
    lightDataSetter.setBias(result, id);
    ShaderProgram::reset();
}

void createDirLamp(DirLamp &result,
        const glm::vec3 &pos, const glm::vec3 &rotate,
        const glm::vec3 &color,
        usize id) {
    result.setPos(pos);
    result.translate();
    result.setRotate(rotate.y, rotate.x, rotate.z);
    result.rotate();
    result.updateMatrix();

    result.setColor(color);
    result.setKc(1.0f);
    result.setKl(0.045f);
    result.setKq(0.0075f);

    result.orthoShadows(-10.0f, 10.0f, -10.0f, 10.0f);
    result.updateMatrix();
    result.initShadows(SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION);

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

void
createShapes(const string &path, const string &texPath, const size_t id, const bool inverseNormals = false,
             uint bonesPerVertex = 0) {
    ShapeLoader shapeLoader;
    shapeLoader.setModelPath(path);
    shapeLoader.setTexturesPath(texPath);
    if (inverseNormals)
        shapeLoader.addParam(ShapeLoader::InverseNormals);
    shapeLoader.addParams(ShapeLoader::Triangulate, ShapeLoader::SortByPolygonType,
            ShapeLoader::CalcTangentSpace, ShapeLoader::JoinIdenticalVertices);
    shapeLoader.getShape()->bonesPerVertex = bonesPerVertex;
    shapeLoader.load();

    shapes[id].reset(shapeLoader.getShape());
    shapes[id]->createInputLayout(
            pointShadowShader->getLocation(ShadowShader::Vars::InPos),
            -1, -1, -1, -1,
            pointShadowShader->getLocation(BoneSystem::Vars::InBoneWeights),
            pointShadowShader->getLocation(BoneSystem::Vars::InBoneIds)
    ); // all shadow shaders have same ids
    shapes[id]->createInputLayout(
            colorShader->getLocation(ColorShader::Vars::InPos),
            colorShader->getLocation(ColorShader::Vars::InTexCoord),
            colorShader->getLocation(ColorShader::Vars::InNormal),
            colorShader->getLocation(ColorShader::Vars::InTangent),
            colorShader->getLocation(ColorShader::Vars::InBitangent),
            colorShader->getLocation(BoneSystem::Vars::InBoneWeights),
            colorShader->getLocation(BoneSystem::Vars::InBoneIds)
    );
}

/* init code begin */

// NOTE: uncomment if you have issues
// send report to mailto:dbcongard@gmail.com
// or create new issue: https://github.com/congard/algine
// #define DEBUG_OUTPUT

/**
 * Init GL: creating window, etc.
 */
void initGL() {
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    // Init GLFW
    if (!glfwInit())
        std::cout << "GLFW init failed";
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#ifdef DEBUG_OUTPUT
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

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

#ifdef DEBUG_OUTPUT
    enableGLDebugOutput();
#endif

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

    std::cout << "Compiling algine shaders\n";

    {
        ShaderManager manager;
        manager.addIncludePath(Path::join(Path::getWorkingDirectory(), "resources"));

        // color shader
        manager.fromFile("resources/shaders/vertex_shader.glsl",
                         "resources/shaders/fragment_shader.glsl");
        manager.define(ColorShader::Settings::Lighting);
        manager.define(ColorShader::Settings::LightingAttenuation);
        manager.define(ColorShader::Settings::NormalMapping);
        manager.define(ColorShader::Settings::ShadowMappingPCF);
        manager.define(ColorShader::Settings::TextureMapping);
        manager.define(BoneSystem::Settings::BoneSystem);
        manager.define(ColorShader::Settings::SSR);
        manager.define(ColorShader::Settings::PointLightsLimit, std::to_string(pointLightsLimit));
        manager.define(ColorShader::Settings::DirLightsLimit, std::to_string(dirLightsLimit));
        manager.define(BoneSystem::Settings::MaxBoneAttribsPerVertex, std::to_string(maxBoneAttribsPerVertex));
        manager.define(BoneSystem::Settings::MaxBones, std::to_string(maxBones));
        colorShader->fromSource(manager.makeGenerated());
        colorShader->loadActiveLocations();

        // point shadow shader
        manager.fromFile("resources/shaders/shadow/vertex_shadow_shader.glsl",
                         "resources/shaders/shadow/fragment_shadow_shader.glsl",
                         "resources/shaders/shadow/geometry_shadow_shader.glsl");
        manager.resetDefinitions();
        manager.define(BoneSystem::Settings::BoneSystem);
        manager.define(BoneSystem::Settings::MaxBoneAttribsPerVertex, std::to_string(maxBoneAttribsPerVertex));
        manager.define(BoneSystem::Settings::MaxBones, std::to_string(maxBones));
        manager.define(ShadowShader::Settings::PointLightShadowMapping);
        pointShadowShader->fromSource(manager.makeGenerated());
        pointShadowShader->loadActiveLocations();

        // dir shadow shader
        ShadersData shadowShaderTemplate = manager.getTemplate();
        shadowShaderTemplate.geometry = std::string(); // we don't need geometry shader for dir light shadows
        manager.fromSource(shadowShaderTemplate);
        manager.removeDefinition(ShadowShader::Settings::PointLightShadowMapping);
        manager.define(ShadowShader::Settings::DirLightShadowMapping);
        dirShadowShader->fromSource(manager.makeGenerated());
        dirShadowShader->loadActiveLocations();

        // SSR shader
        ssrShader->fromFile("resources/shaders/basic/quad_vertex.glsl",
                            "resources/shaders/ssr/fragment.glsl");
        ssrShader->loadActiveLocations();

        // bloom search shader
        bloomSearchShader->fromFile("resources/shaders/basic/quad_vertex.glsl",
                                    "resources/shaders/bloom/fragment_search.glsl");
        bloomSearchShader->loadActiveLocations();

        // TODO: remove "dof blur shaders", replace with only blur + blend steps
        // DOF blur shaders
        manager.fromFile("resources/shaders/basic/quad_vertex.glsl",
                         "resources/shaders/dof/fragment.glsl");
        manager.resetDefinitions();
        manager.define(DOFShaders::Settings::DofCocMap);
        manager.define(BlurShader::Settings::KernelRadius, std::to_string(dofBlurKernelRadius));
        manager.define(DOFShaders::Settings::BleedingMinDeltaZ);
        manager.define(DOFShaders::Settings::BleedingMinDeltaCoC);
        manager.define(DOFShaders::Settings::BleedingMaxFocusCoC);
        manager.define(BlurShader::Settings::Horizontal);
        dofBlurHorShader->fromSource(manager.makeGenerated());
        dofBlurHorShader->loadActiveLocations();

        manager.resetGenerated();
        manager.removeDefinition(BlurShader::Settings::Horizontal);
        manager.define(BlurShader::Settings::Vertical);
        dofBlurVertShader->fromSource(manager.makeGenerated());
        dofBlurVertShader->loadActiveLocations();

        // DOF CoC shader
        manager.fromFile("resources/shaders/basic/quad_vertex.glsl",
                         "resources/shaders/dof/coc_fragment.glsl");
        manager.resetDefinitions();
        manager.define(DOFShaders::Settings::CinematicDof);
        dofCoCShader->fromSource(manager.makeGenerated());
        dofCoCShader->loadActiveLocations();

        // blend shader
        manager.fromFile("resources/shaders/basic/quad_vertex.glsl",
                         "resources/shaders/blend/fragment.glsl");
        manager.resetDefinitions();
        manager.define(BlendBloomModule::Settings::BloomAdd);
        blendShader->fromSource(manager.makeGenerated());
        blendShader->loadActiveLocations();

        // bloom blur shaders
        manager.fromFile("resources/shaders/basic/quad_vertex.glsl",
                         "resources/shaders/blur/fragment.glsl");
        manager.resetDefinitions();
        manager.define(BlurShader::Settings::KernelRadius, std::to_string(bloomBlurKernelRadius));
        manager.define(BlurShader::Settings::OutputType, "vec3");
        manager.define(BlurShader::Settings::TexComponent, "rgb");
        manager.define(BlurShader::Settings::Horizontal);
        bloomBlurHorShader->fromSource(manager.makeGenerated());
        bloomBlurHorShader->loadActiveLocations();

        manager.resetGenerated();
        manager.removeDefinition(BlurShader::Settings::Horizontal);
        manager.define(BlurShader::Settings::Vertical);
        bloomBlurVertShader->fromSource(manager.makeGenerated());
        bloomBlurVertShader->loadActiveLocations();

        // CoC blur shaders
        manager.resetGenerated();
        manager.resetDefinitions();
        manager.define(BlurShader::Settings::KernelRadius, std::to_string(cocBlurKernelRadius));
        manager.define(BlurShader::Settings::OutputType, "float");
        manager.define(BlurShader::Settings::TexComponent, "r");
        manager.define(BlurShader::Settings::Horizontal);
        cocBlurHorShader->fromSource(manager.makeGenerated());
        cocBlurHorShader->loadActiveLocations();

        manager.resetGenerated();
        manager.removeDefinition(BlurShader::Settings::Horizontal);
        manager.define(BlurShader::Settings::Vertical);
        cocBlurVertShader->fromSource(manager.makeGenerated());
        cocBlurVertShader->loadActiveLocations();

        // cubemap shader
        manager.fromFile("resources/shaders/basic/cubemap_vertex.glsl",
                         "resources/shaders/basic/cubemap_fragment.glsl");
        manager.resetDefinitions();
        manager.define(CubemapShader::Settings::SpherePositions);
        manager.define(CubemapShader::Settings::ColorOut, "0"); // TODO: create constants
        manager.define(CubemapShader::Settings::PosOut, "2");
        manager.define(CubemapShader::Settings::OutputType, "vec3");
        skyboxShader->fromSource(manager.makeGenerated());
        skyboxShader->loadActiveLocations();
    }

    std::cout << "Compilation done\n";

    #define value *

    lightDataSetter.indexDirLightLocations(colorShader, dirLightsLimit);
    lightDataSetter.indexPointLightLocations(colorShader, pointShadowShader, pointLightsLimit);

    skyboxRenderer = make_shared<CubeRenderer>(skyboxShader->getLocation(CubemapShader::Vars::InPos));
    quadRenderer = make_shared<QuadRenderer>(0); // inPosLocation in quad shader is 0

    renderer.blendShader = blendShader;
    renderer.dofBlurShaders[0] = dofBlurHorShader;
    renderer.dofBlurShaders[1] = dofBlurVertShader;
    renderer.dofCoCShader = dofCoCShader;
    renderer.quadRenderer = quadRenderer.get();

    Framebuffer::create(displayFb, screenspaceFb, bloomSearchFb, pingpongFb[0], pingpongFb[1], cocFb);

    Renderbuffer::create(rbo);

    Texture2D::create(colorTex, normalTex, ssrValues, positionTex, screenspaceTex, bloomTex,
                      pingpongDofTex[0], pingpongDofTex[1], cocTex);
    ssrValues->setFormat(Texture::RG16F);
    cocTex->setFormat(Texture::Red16F);

    TextureCube::create(skybox);
    skybox->setFormat(Texture::RGB);
    skybox->bind();
    skybox->fromFile("resources/skybox/right.tga", TextureCube::Right);
    skybox->fromFile("resources/skybox/left.tga", TextureCube::Left);
    skybox->fromFile("resources/skybox/top.jpg", TextureCube::Top);
    skybox->fromFile("resources/skybox/bottom.tga", TextureCube::Bottom);
    skybox->fromFile("resources/skybox/front.tga", TextureCube::Front);
    skybox->fromFile("resources/skybox/back.tga", TextureCube::Back);
    skybox->setParams(TextureCube::defaultParams());
    skybox->unbind();

    Texture2D::setParamsMultiple(Texture2D::defaultParams(),
                                 colorTex, normalTex, ssrValues, positionTex, screenspaceTex, bloomTex,
                                 pingpongDofTex[0], pingpongDofTex[1], cocTex);

    TextureCreateInfo createInfo;
    createInfo.format = Texture::RGB16F;
    createInfo.width = winWidth * bloomK;
    createInfo.height = winHeight * bloomK;
    createInfo.params = Texture2D::defaultParams();

    bloomBlur = make_shared<Blur>(createInfo);
    bloomBlur->setPingPongShaders(bloomBlurHorShader, bloomBlurVertShader);
    bloomBlur->setQuadRenderer(quadRenderer.get());
    bloomBlur->configureKernel(bloomBlurKernelRadius, bloomBlurKernelSigma);

    createInfo.format = Texture::Red16F;
    createInfo.width = winWidth;
    createInfo.height = winHeight;

    cocBlur = make_shared<Blur>(createInfo);
    cocBlur->setPingPongShaders(cocBlurHorShader, cocBlurVertShader);
    cocBlur->setQuadRenderer(quadRenderer.get());
    cocBlur->configureKernel(cocBlurKernelRadius, cocBlurKernelSigma);

    updateRenderTextures();

    rbo->bind();
    rbo->setWidthHeight(winWidth, winHeight);
    rbo->setFormat(Texture::DepthComponent);
    rbo->update();
    rbo->unbind();

    displayFb->bind();
    displayFb->attachRenderbuffer(rbo, Framebuffer::DepthAttachment);

    displayFb->addOutput(0, Framebuffer::ColorAttachmentZero + 0);
    displayFb->addOutput(1, Framebuffer::ColorAttachmentZero + 1);
    displayFb->addOutput(2, Framebuffer::ColorAttachmentZero + 2);
    displayFb->addOutput(3, Framebuffer::ColorAttachmentZero + 3);

    displayFb->addAttachmentsList();
    displayFb->setActiveAttachmentsList(1);
    displayFb->addOutput(0, Framebuffer::ColorAttachmentZero + 0);
    displayFb->addOutput(2, Framebuffer::ColorAttachmentZero + 2);
    // displayFb->update(); // not need here because it will be called each frame

    displayFb->attachTexture(colorTex, Framebuffer::ColorAttachmentZero + 0);
    displayFb->attachTexture(normalTex, Framebuffer::ColorAttachmentZero + 1);
    displayFb->attachTexture(positionTex, Framebuffer::ColorAttachmentZero + 2);
    displayFb->attachTexture(ssrValues, Framebuffer::ColorAttachmentZero + 3);

    for (size_t i = 0; i < 2; i++) {
        // configuring ping-pong (blur)
        pingpongFb[i]->bind();
        pingpongFb[i]->attachTexture(pingpongDofTex[i], Framebuffer::ColorAttachmentZero);
    }

    screenspaceFb->bind();
    screenspaceFb->attachTexture(screenspaceTex, Framebuffer::ColorAttachmentZero);

    bloomSearchFb->bind();
    bloomSearchFb->attachTexture(bloomTex, Framebuffer::ColorAttachmentZero);

    cocFb->bind();
    cocFb->attachTexture(cocTex, Framebuffer::ColorAttachmentZero);
    cocFb->unbind();

    // configuring CS
    colorShader->use();
    colorShader->setInt(ColorShader::Vars::Material::AmbientTex, 0);
    colorShader->setInt(ColorShader::Vars::Material::DiffuseTex, 1);
    colorShader->setInt(ColorShader::Vars::Material::SpecularTex, 2);
    colorShader->setInt(ColorShader::Vars::Material::NormalTex, 3);
    colorShader->setInt(ColorShader::Vars::Material::ReflectionStrengthTex, 4);
    colorShader->setInt(ColorShader::Vars::Material::JitterTex, 5);
    colorShader->setFloat(ColorShader::Vars::ShadowOpacity, shadowOpacity);

    // configuring CubemapShader
    skyboxShader->use();
    skyboxShader->setVec3(CubemapShader::Vars::Color, glm::vec3(0.125f));
    skyboxShader->setFloat(CubemapShader::Vars::PosScaling, 64.0f);

    // blend setting
    blendShader->use();
    blendShader->setInt(BlendBloomModule::Vars::BaseImage, 0); // GL_TEXTURE0
    blendShader->setInt(BlendBloomModule::Vars::BloomImage, 1); // GL_TEXTURE1
    blendShader->setFloat(BlendBloomModule::Vars::Exposure, blendExposure);
    blendShader->setFloat(BlendBloomModule::Vars::Gamma, blendGamma);

    // dof blur setting
    for (size_t i = 0; i < 2; i++) {
        renderer.dofBlurShaders[i]->use();
        renderer.dofBlurShaders[i]->setInt(DOFShaders::Vars::BaseImage, 0);
        renderer.dofBlurShaders[i]->setInt(DOFShaders::Vars::CoCMap, 1);
        renderer.dofBlurShaders[i]->setInt(DOFShaders::Vars::PositionMap, 2);
        renderer.dofBlurShaders[i]->setFloat(DOFShaders::Vars::BleedingMinDeltaZ, bleedingEliminationMinDeltaZ);
        renderer.dofBlurShaders[i]->setFloat(DOFShaders::Vars::BleedingMinDeltaCoC, bleedingEliminationMinDeltaCoC);
        renderer.dofBlurShaders[i]->setFloat(DOFShaders::Vars::BleedingMaxFocusCoC, bleedingEliminationMaxFocusCoC);
    }

    // screen space setting
    ssrShader->use();
    ssrShader->setInt(SSRShader::Vars::NormalMap, 1);
    ssrShader->setInt(SSRShader::Vars::SSRValuesMap, 2);
    ssrShader->setInt(SSRShader::Vars::PositionMap, 3);
    ssrShader->reset();
}

/**
 * Creating matrices
 */
void initCamera() {
    camera.setFar(64.0f);
    camera.setFieldOfView(glm::radians(60.0f));
    camera.setAspectRatio((float)winWidth / (float)winHeight);
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
    string path = "resources/models/";

    createShapes(path + "chess/Classic Chess small.obj", path + "chess", 0, false, 0); // classic chess
    createShapes(path + "japanese_lamp/japanese_lamp.obj", path + "japanese_lamp", 1, true, 0); // Japanese lamp
    createShapes(path + "man/man.dae", path + "man", 2, false, 4); // animated man
    createShapes(path + "astroboy/astroboy_walk.dae", path + "astroboy", 3, false, 4);
}

/**
 * Creating models from created buffers and loaded textures
 */
void createModels() {
    // classic chess
    models[0] = Model(Rotator::RotatorTypeSimple);
    models[0].shape = shapes[0].get();

    // animated man
    manAnimator = Animator(AnimShape(&shapes[2]->animations, &shapes[2]->bones, &shapes[2]->globalInverseTransform, &shapes[2]->rootNode));
    models[1] = Model(Rotator::RotatorTypeSimple);
    models[1].shape = shapes[2].get();
    models[1].setPitch(glm::radians(-90.0f));
    models[1].rotate();
    models[1].setX(-2.0f);
    models[1].translate();
    models[1].updateMatrix();
    models[1].animator = &manAnimator;

    // animated astroboy
    astroboyAnimator = Animator(AnimShape(&shapes[3]->animations, &shapes[3]->bones, &shapes[3]->globalInverseTransform, &shapes[3]->rootNode));
    models[2] = Model(Rotator::RotatorTypeSimple);
    models[2].shape = shapes[3].get();
    models[2].setPitch(glm::radians(-90.0f));
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
    lamps[0] = Model(Rotator::RotatorTypeSimple);
    lamps[0].shape = shapes[1].get();
    pointLamps[0].mptr = &lamps[0];
    createPointLamp(pointLamps[0], glm::vec3(0.0f, 8.0f, 15.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0);
    lamps[0].translate();
    lamps[0].updateMatrix();

    lamps[1] = Model(Rotator::RotatorTypeSimple);
    lamps[1].shape = shapes[1].get();
    dirLamps[0].mptr = &lamps[1];
    createDirLamp(dirLamps[0],
            glm::vec3(0.0f, 8.0f, -15.0f),
            glm::vec3(glm::radians(180.0f), glm::radians(30.0f), 0.0f),
            glm::vec3(253.0f / 255.0f, 184.0f / 255.0f, 19.0f / 255.0f), 0);
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
    // TODO: move it to LightDataSetter
    for (int i = 0; i < pointLightsLimit; i++) {
        ShaderProgram::setInt(lightDataSetter.getLocation(LightDataSetter::ShadowMap, Light::TypePointLight, i), POINT_LIGHT_TSID + i);
		glActiveTexture(GL_TEXTURE0 + POINT_LIGHT_TSID + i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    for (usize i = 0; i < pointLampsCount; i++)
        lightDataSetter.setShadowMap(pointLamps[i], i, POINT_LIGHT_TSID + i);

    // to avoid black screen on AMD GPUs and old Intel HD Graphics
    // Note: Mesa drivers require int as sampler, not uint
    for (int i = 0; i < dirLightsLimit; i++) {
        ShaderProgram::setInt(lightDataSetter.getLocation(LightDataSetter::ShadowMap, Light::TypeDirLight, i), DIR_LIGHT_TSID + i);
		glActiveTexture(GL_TEXTURE0 + DIR_LIGHT_TSID + i);
		glBindTexture(GL_TEXTURE_2D, 0);
    }
    for (usize i = 0; i < dirLampsCount; i++)
        lightDataSetter.setShadowMap(dirLamps[i], i, DIR_LIGHT_TSID + i);
    glUseProgram(0);
}

void initShadowCalculation() {
    colorShader->use();
    colorShader->setFloat(ColorShader::Vars::ShadowDiskRadiusK, diskRadius_k);
    colorShader->setFloat(ColorShader::Vars::ShadowDiskRadiusMin, diskRadius_min);
    glUseProgram(0);
}

/**
 * Initialize Depth of field
 */
void initDOF() {
    dofCoCShader->use();
    //dofCoCShader->set(ALGINE_DOF_SIGMA_MAX, dof_max_sigma);
    //dofCoCShader->set(ALGINE_DOF_SIGMA_MIN, dof_min_sigma);
    dofCoCShader->setFloat(DOFShaders::Vars::Aperture, dofAperture);
    dofCoCShader->setFloat(DOFShaders::Vars::ImageDistance, dofImageDistance);
    dofCoCShader->setFloat(DOFShaders::Vars::PlaneInFocus, -1.0f);
    glUseProgram(0);
}

/* init code end */

/**
 * Cleans memory before exit
 */
void recycleAll() {
    for (size_t i = 0; i < SHAPES_COUNT; i++)
        shapes[i]->recycle();

    Framebuffer::destroy(displayFb, screenspaceFb, bloomSearchFb, pingpongFb[0], pingpongFb[1], cocFb);

    Texture2D::destroy(colorTex, normalTex, ssrValues, positionTex, screenspaceTex, bloomTex,
                       pingpongDofTex[0], pingpongDofTex[1], cocTex);
    TextureCube::destroy(skybox);

    Renderbuffer::destroy(rbo);
}

void sendLampsData() {
    lightDataSetter.setPointLightsCount(pointLampsCount);
    lightDataSetter.setDirLightsCount(dirLampsCount);
    colorShader->setVec3(ColorShader::Vars::CameraPos, camera.getPos());
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
    colorShader->setMat4(ColorShader::Vars::MVPMatrix, getMVPMatrix());
    colorShader->setMat4(ColorShader::Vars::MVMatrix, getMVMatrix());
    colorShader->setMat4(ColorShader::Vars::ModelMatrix, value modelMatrix);
    colorShader->setMat4(ColorShader::Vars::ViewMatrix, camera.getViewMatrix());
}

/**
 * Draws model in depth map<br>
 * if point light, leave mat empty, but if dir light - it must be light space matrix
 */
void drawModelDM(const Model &model, ShaderProgram *program, const glm::mat4 &mat = glm::mat4(1.0f)) {
    model.shape->inputLayouts[0]->bind();

    if (model.shape->bonesPerVertex != 0) {
        for (int i = 0; i < model.shape->bones.size(); i++) {
            program->setMat4(program->getLocation(BoneSystem::Vars::Bones) + i, model.shape->bones[i].finalTransformation);
        }
    }

    program->setInt(BoneSystem::Vars::BoneAttribsPerVertex, (int)(model.shape->bonesPerVertex / 4 + (model.shape->bonesPerVertex % 4 == 0 ? 0 : 1)));
    program->setMat4(ShadowShader::Vars::TransformationMatrix, mat * model.m_transform);
    
    for (size_t i = 0; i < model.shape->meshes.size(); i++) {
        glDrawElements(GL_TRIANGLES, model.shape->meshes[i].count, GL_UNSIGNED_INT, reinterpret_cast<void*>(model.shape->meshes[i].start * sizeof(uint)));
    }
}

/**
 * Draws model
 */
#define useNotNull(tex, slot) \
if (tex != nullptr) \
    tex->use(slot); \
else \
    texture2DAB(slot, 0);
void drawModel(const Model &model) {
    model.shape->inputLayouts[1]->bind();
    
    if (model.shape->bonesPerVertex != 0) {
        for (int i = 0; i < model.shape->bones.size(); i++) {
            colorShader->setMat4(colorShader->getLocation(BoneSystem::Vars::Bones) + i, model.shape->bones[i].finalTransformation);
        }
    }

    colorShader->setInt(BoneSystem::Vars::BoneAttribsPerVertex, model.shape->bonesPerVertex / 4 + (model.shape->bonesPerVertex % 4 == 0 ? 0 : 1));
    modelMatrix = &model.m_transform;
	updateMatrices();
    for (size_t i = 0; i < model.shape->meshes.size(); i++) {
        useNotNull(model.shape->meshes[i].material.ambientTexture, 0);
        useNotNull(model.shape->meshes[i].material.diffuseTexture, 1);
        useNotNull(model.shape->meshes[i].material.specularTexture, 2);
        useNotNull(model.shape->meshes[i].material.normalTexture, 3);
        useNotNull(model.shape->meshes[i].material.reflectionTexture, 4);
        useNotNull(model.shape->meshes[i].material.jitterTexture, 5);

        colorShader->setFloat(ColorShader::Vars::Material::AmbientStrength, model.shape->meshes[i].material.ambientStrength);
        colorShader->setFloat(ColorShader::Vars::Material::DiffuseStrength, model.shape->meshes[i].material.diffuseStrength);
        colorShader->setFloat(ColorShader::Vars::Material::SpecularStrength, model.shape->meshes[i].material.specularStrength);
        colorShader->setFloat(ColorShader::Vars::Material::Shininess, model.shape->meshes[i].material.shininess);

        glDrawElements(GL_TRIANGLES, model.shape->meshes[i].count, GL_UNSIGNED_INT, reinterpret_cast<void*>(model.shape->meshes[i].start * sizeof(uint)));
    }
}

/**
 * Renders to depth cubemap
 */
void renderToDepthCubemap(const uint index) {
	pointLamps[index].begin();
    pointLamps[index].updateMatrix();
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
        drawModelDM(models[i], dirShadowShader, dirLamps[index].m_lightSpace);

	// drawing lamps
	for (GLuint i = 0; i < dirLampsCount; i++) {
		if (i == index) continue;
        drawModelDM(*dirLamps[i].mptr, dirShadowShader, dirLamps[index].m_lightSpace);
	}

	dirLamps[index].end();
}

/**
 * Color rendering
 */
void render() {
    displayFb->bind();
    displayFb->setActiveAttachmentsList(0);
    displayFb->update();

    // TODO: glClear, glViewport etc must be part of Application (or AlgineApplication?) class
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, winWidth, winHeight);

    colorShader->use();

    // sending lamps parameters to fragment shader
	sendLampsData();

    // drawing
    for (size_t i = 0; i < MODELS_COUNT; i++)
        drawModel(models[i]);
	for (size_t i = 0; i < pointLampsCount + dirLampsCount; i++)
	    drawModel(lamps[i]);

    // render skybox
    displayFb->setActiveAttachmentsList(1);
    displayFb->update();

    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();
    skyboxShader->setMat3(CubemapShader::Vars::ViewMatrix, glm::mat3(camera.getViewMatrix()));
    skyboxShader->setMat4(CubemapShader::Vars::TransformationMatrix, camera.getProjectionMatrix() * glm::mat4(glm::mat3(camera.getViewMatrix())));
    skybox->use(0);
    skyboxRenderer->getInputLayout()->bind();
    skyboxRenderer->draw();
    glDepthFunc(GL_LESS);

    // postprocessing
    quadRenderer->getInputLayout()->bind();

    screenspaceFb->bind();
    ssrShader->use();
    colorTex->use(0);
    normalTex->use(1);
    ssrValues->use(2);
    positionTex->use(3);
    quadRenderer->draw();

    glViewport(0, 0, winWidth * bloomK, winHeight * bloomK);
    bloomSearchFb->bind();
    bloomSearchShader->use();
    screenspaceTex->use(0);
    quadRenderer->draw();
    bloomBlur->makeBlur(bloomTex);
    glViewport(0, 0, winWidth, winHeight);

    // TODO: tmp, AlgineRenderer must be eliminated !!!!!!
    // Now it looks like shit code... Oh I know

    renderer.dofCoCPass(cocFb->getId(), positionTex->getId());

    cocBlur->makeBlur(cocTex);

    uint ids[] = {pingpongDofTex[0]->getId(), pingpongDofTex[1]->getId()};
    uint fbos[] = {pingpongFb[0]->getId(), pingpongFb[1]->getId()};
    renderer.dofBlurPass(fbos, ids, screenspaceTex->getId(), cocBlur->get()->getId(), positionTex->getId(), dofBlurAmount);

    bindFramebuffer(0);
    glClear(GL_DEPTH_BUFFER_BIT); // color will cleared by quad rendering
    renderer.doubleBlendPass(pingpongDofTex[!renderer.horizontal]->getId(), bloomBlur->get()->getId());
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
    ssrShader->setMat4(SSRShader::Vars::ProjectionMatrix, camera.getProjectionMatrix());
    ssrShader->setMat4(SSRShader::Vars::ViewMatrix, camera.getViewMatrix());

	/* --- color rendering --- */
	render();
	glUseProgram(0);
}

void animate_scene() {
    glm::mat3 rotate = glm::mat3(glm::rotate(glm::mat4(), glm::radians(0.01f), glm::vec3(0, 1, 0)));
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        pointLamps[0].setPos(pointLamps[0].m_pos * rotate);
        pointLamps[0].translate();
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

    std::thread animate_scene_th(animate_scene);

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

    animate_scene_th.detach();
    recycleAll();
    glfwTerminate(); // Terminate GLFW, clearing any resources allocated by GLFW.
    
    std::cout << "Exit with exit code 0\n";

    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        GLfloat *pixels = getTexImage2D(dirLamps[0].shadowMap->id, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, GL_DEPTH_COMPONENT);
        saveTexImage(pixels, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 1, tulz::Path::getWorkingDirectory() + "/out/dir_depth.bmp", 3);
        delete[] pixels;
        std::cout << "Depth map data saved\n";
    }
    else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
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
            shapes[2]->setNodeTransform("Armature_Head", r);
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
        
            GLfloat *pixels = getPixels(positionTex->getId(), (size_t)event->getX(), winHeight - (size_t)event->getY(), 1, 1, GL_RGB);
        
            std::cout << "x: " << pixels[0] << "; y: " << pixels[1] << "; z: " << pixels[2] << "\n";
        
            dofCoCShader->use();
            dofCoCShader->setFloat(DOFShaders::Vars::PlaneInFocus, pixels[2] == 0 ? FLT_EPSILON : pixels[2]);
            glUseProgram(0);
        
            delete[] pixels;
            break;
    }
}
