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

#define FULLSCREEN !true

#define LAMPS_COUNT 1
#define SHAPES_COUNT 4
#define MODELS_COUNT 3

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);

// Window dimensions
GLuint WIN_W = 1366, WIN_H = 763;
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
Model models[MODELS_COUNT], lamps[LAMPS_COUNT];
Animator manAnimator, astroboyAnimator; // animator for man, astroboy models

// light
Lamp lightSources[LAMPS_COUNT];

// shadow maps
TextureArray shadowMaps;

// renderer
AlgineRenderer renderer;

GLuint
    displayFB,
    screenspaceFB,
    pingpongFB[2],
    rbo;

GLuint
    colorTex,
    dofTex,
    normalTex,
    ssrValues,
    positionTex,
    screenspaceTex,
    bloomTex,
    pingpongBloomTex[2],
    pingpongDofTex[2];

CShader cs;
SShader ss;
BLUShader blusH, blusV;
BLEShader bles;
SSShader sss;

AlgineParams params;

float
    // DOF variables
    dofImageDistance = 1.0f,
    dofAperture = 20.0f,
    dof_max_sigma = 6.0f,
    dof_min_sigma = 0.0001f,

    // diskRadius variables
    diskRadius_k = 1.0f / 25.0f,
    diskRadius_min = 0.0f,

    // other shadow variables
    shadow_bias = 0.3f,
    
    // shadow opacity: 1.0 - opaque shadow (by default), 0.0 - transparent
    shadowOpacity = 0.65f;

void createProjectionMatrix() {
    projectionMatrix = glm::perspective(glm::radians(90.0f), (float) WIN_W / (float) WIN_H, 1.0f, 32.0f);
}

void createViewMatrix() {
    viewMatrix = glm::lookAt(cameraPos, cameraLookAt, cameraUp);
}

/**
 * To correctly display the scene when changing the window size
 */
void window_size_callback(GLFWwindow* window, int width, int height) {
    WIN_W = width;
    WIN_H = height;
    params.scrW = WIN_W;
    params.scrH = WIN_H;
    createProjectionMatrix();
}

/**
 * Creates Lamp with default params
 */
void createLamp(Lamp &result, const glm::vec3 &pos, const glm::vec3 &color, GLuint id) {
    result.pos = pos;
    result.color = color;
    result.kc = 1;
    result.kl = 0.045f;
    result.kq = 0.0075f;

    result.loadLocations(cs.programId, id);
    result.initShadowMapping(SHADOW_MAP_RESOLUTION);
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
        glfwCreateWindow(WIN_W, WIN_H, "Algine", glfwGetPrimaryMonitor(), nullptr) :
        glfwCreateWindow(WIN_W, WIN_H, "Algine", nullptr, nullptr);

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
	glDepthMask(true);
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
    #endif
    params.dofKernelSize = 4;
    params.dofMode = ALGINE_CINEMATIC_DOF_MODE_ENABLED;
    params.boneSystemMode = ALGINE_BONE_SYSTEM_ENABLED;
    params.exposure = 6.0f;
    params.gamma = 1.125f;
    cs.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getCS(
        params,
        "src/resources/shaders/vertex_shader.glsl",
        "src/resources/shaders/fragment_shader.glsl")
    ));
    ss.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getSS(
        params,
        "src/resources/shaders/vertex_shadow_shader.glsl",
        "src/resources/shaders/fragment_shadow_shader.glsl",
        "src/resources/shaders/geometry_shadow_shader.glsl")
    ));
    sss.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getSSS(
        params,
        "src/resources/shaders/vertex_screenspace_shader.glsl",
        "src/resources/shaders/fragment_screenspace_shader.glsl")
    ));
    std::vector<ShadersData> blus = scompiler::getBLUS(
        params,
        "src/resources/shaders/vertex_blur_shader.glsl",
        "src/resources/shaders/fragment_blur_shader.glsl"
    );
    blusH.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(
        blus[0]
    ));
    blusV.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(
        blus[1]
    ));
    bles.programId = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getBLES(
        params,
        "src/resources/shaders/vertex_blend_shader.glsl",
        "src/resources/shaders/fragment_blend_shader.glsl")
    ));
    scompiler::loadLocations(cs);
    scompiler::loadLocations(ss);
    scompiler::loadLocations(sss);
    scompiler::loadLocations(blusH, params);
    scompiler::loadLocations(blusV, params);
    scompiler::loadLocations(bles);

    renderer.sss = &sss;
    renderer.bles = &bles;
    renderer.blusHV[0] = &blusH;
    renderer.blusHV[1] = &blusV;
    renderer.params = &params;
    renderer.prepare();

    Framebuffer::create(&displayFB);
    Framebuffer::create(&screenspaceFB);
    Framebuffer::create(pingpongFB, 2);
    glGenRenderbuffers(1, &rbo);

    Texture::create(&colorTex);
    Texture::create(&dofTex);
    Texture::create(&normalTex);
    Texture::create(&ssrValues);
    Texture::create(&positionTex);
    Texture::create(&screenspaceTex);
    Texture::create(&bloomTex);
    Texture::create(pingpongBloomTex, 2);
    Texture::create(pingpongDofTex, 2);

    applyDefaultTexture2DParams(colorTex);
    applyDefaultTexture2DParams(dofTex);
    applyDefaultTexture2DParams(normalTex);
    applyDefaultTexture2DParams(ssrValues);
    applyDefaultTexture2DParams(positionTex);
    applyDefaultTexture2DParams(screenspaceTex);
    applyDefaultTexture2DParams(bloomTex);
    applyDefaultTexture2DParams(pingpongBloomTex, 2);
    applyDefaultTexture2DParams(pingpongDofTex, 2);

    GLuint displayColorAttachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    bindFramebuffer(displayFB);
    glDrawBuffers(5, displayColorAttachments);

    Framebuffer::attachTexture2D(colorTex, COLOR_ATTACHMENT(0));
    Framebuffer::attachTexture2D(dofTex, COLOR_ATTACHMENT(1));
    Framebuffer::attachTexture2D(normalTex, COLOR_ATTACHMENT(2));
    Framebuffer::attachTexture2D(ssrValues, COLOR_ATTACHMENT(3));
    Framebuffer::attachTexture2D(positionTex, COLOR_ATTACHMENT(4));

    GLuint colorAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    for (size_t i = 0; i < 2; i++) {
        // configuring ping-pong (blur) textures
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFB[i]);
        glDrawBuffers(2, colorAttachments); // use 2 color components (boom / dof)

        Framebuffer::attachTexture2D(pingpongBloomTex[i], COLOR_ATTACHMENT(0));
        Framebuffer::attachTexture2D(pingpongDofTex[i], COLOR_ATTACHMENT(1));
    }

    // sending to shader center of kernel and right part
    float kernel[(params.bloomKernelSize - 1) * 2];
    getGB1DKernel(kernel, (params.bloomKernelSize - 1) * 2, params.bloomKernelSigma);

    for (size_t j = 0; j < 2; j++) {
        glUseProgram(renderer.blusHV[j]->programId);
        for (GLuint i = 0; i < params.bloomKernelSize; i++)
            glUniform1f(
                j ? blusV.bloomKernel[params.bloomKernelSize - 1 - i] : blusH.bloomKernel[params.bloomKernelSize - 1 - i],
                kernel[i]);
    }

    glUseProgram(0);

    bindFramebuffer(screenspaceFB);
    glDrawBuffers(2, colorAttachments);
    Framebuffer::attachTexture2D(screenspaceTex, COLOR_ATTACHMENT(0));
    Framebuffer::attachTexture2D(bloomTex, COLOR_ATTACHMENT(1));

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
    Light::lightPosLocation = ss.lampPos;
    Light::initShadowMatrices(ss.programId);

    createLamp(lightSources[0], glm::vec3(0.0f, 8.0f, 15.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0);
    lamps[0].shape = &shapes[1];
    lightSources[0].mptr = &lamps[0];
    lamps[0].translate(lightSources[0].pos);
    lamps[0].transform();
}

/**
 * Binds to depth cubemaps
 */
void initShadowMaps() {
    shadowMaps = TextureArray(10, params.maxLampsCount); // to avoid black screen on AMD GPUs and old Intel HD Graphics
    shadowMaps.fillTexIds(0); // to avoid black screen on AMD GPUs and old Intel HD Graphics
    shadowMaps.init(cs.programId, ALGINE_NAME_CS_SHADOW_MAPS);
    for (size_t i = 0; i < LAMPS_COUNT; i++) shadowMaps.addTexture(i, lightSources[i].depthCubemap);
    shadowMaps.bind(cs.programId);
}

void initShadowCalculation() {
    glUseProgram(cs.programId);
    glUniform1f(cs.shadowDiskRadiusK, diskRadius_k);
    glUniform1f(cs.shadowDiskRadiusMin, diskRadius_min);
    glUniform1f(cs.shadowBias, shadow_bias);
    glUseProgram(0);
}

/**
 * Initialize Depth of field
 */
void initDOF() {
    glUseProgram(blusH.programId);
    glUniform1f(blusH.sigmaMax, dof_max_sigma);
    glUniform1f(blusH.sigmaMin, dof_min_sigma);
    glUseProgram(blusV.programId);
    glUniform1f(blusV.sigmaMax, dof_max_sigma);
    glUniform1f(blusV.sigmaMin, dof_min_sigma);
    glUseProgram(cs.programId);
    glUniform1f(cs.cinematicDOFAperture, dofAperture);
    glUniform1f(cs.cinematicDOFImageDistance, dofImageDistance);
    glUniform1f(cs.cinematicDOFPlaneInFocus, -1.0f);
    glUseProgram(0);
}

/* init code end */

/**
 * Cleans memory before exit
 */
void recycleAll() {
    glDeleteVertexArrays(1, &vao);
    for (size_t i = 0; i < SHAPES_COUNT; i++) shapes[i].recycle();
}

void sendLampsData() {
	glUniform1i(cs.lampsCount, LAMPS_COUNT); // lamps count
	setVec3(cs.viewPos, cameraPos); // current camera position
	for (size_t i = 0; i < LAMPS_COUNT; i++) lightSources[i].pushAll();
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
 * @param shaderProgram
 * @param model
 */
void drawModelDM(Model &model) {
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
void renderToDepthMap(GLuint index) {
	lightSources[index].begin();
	lightSources[index].setLightPos();
	lightSources[index].updateMatrices();
	lightSources[index].setShadowMatrices();
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnableVertexAttribArray(ss.inPosition);

	// drawing models
	for (size_t i = 0; i < MODELS_COUNT; i++) drawModelDM(models[i]);

	// drawing lamps
	for (GLuint i = 0; i < LAMPS_COUNT; i++) {
		if (i == index) continue;
		drawModelDM(lamps[i]);
	}

	glDisableVertexAttribArray(ss.inPosition);

	lightSources[index].end();
}

/**
 * Color rendering
 */
void render() {
    renderer.mainPass(displayFB, rbo);
    cfgtex(colorTex, GL_RGBA16F, GL_RGBA, params.scrW, params.scrH);
    cfgtex(dofTex, GL_R16F, GL_RED, params.scrW, params.scrH);
    cfgtex(normalTex, GL_RGB16F, GL_RGB, params.scrW, params.scrH);
    cfgtex(ssrValues, GL_RG16F, GL_RG, params.scrW, params.scrH);
    cfgtex(positionTex, GL_RGB16F, GL_RGB, params.scrW, params.scrH);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearBufferfv(GL_COLOR, 1, ALGINE_RED); // dof buffer

	// view port to window size
	glViewport(0, 0, WIN_W, WIN_H);
	// updating view matrix (because camera position was changed)
	createViewMatrix();
	// sending lamps parameters to fragment shader
	sendLampsData();
    glEnableVertexAttribArray(cs.inPosition);
    glEnableVertexAttribArray(cs.inNormal);
    glEnableVertexAttribArray(cs.inTexCoord);
    glEnableVertexAttribArray(cs.inTangent);
    glEnableVertexAttribArray(cs.inBitangent);

    // drawing
    for (size_t i = 0; i < MODELS_COUNT; i++) drawModel(models[i]);
	for (size_t i = 0; i < LAMPS_COUNT; i++) drawModel(lamps[i]);

    glDisableVertexAttribArray(cs.inPosition);
    glDisableVertexAttribArray(cs.inNormal);
    glDisableVertexAttribArray(cs.inTexCoord);
    glDisableVertexAttribArray(cs.inTangent);
    glDisableVertexAttribArray(cs.inBitangent);

    cfgtex(screenspaceTex, GL_RGB16F, GL_RGB, params.scrW, params.scrH);
    cfgtex(bloomTex, GL_RGB16F, GL_RGB, params.scrW, params.scrH);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    renderer.screenspacePass(screenspaceFB, colorTex, normalTex, ssrValues, positionTex);

    // configuring textures
	for (size_t i = 0; i < 2; i++) {
        cfgtex(pingpongBloomTex[i], GL_RGB16F, GL_RGB, params.scrW, params.scrH); // bloom
		cfgtex(pingpongDofTex[i], GL_RGB16F, GL_RGB, params.scrW, params.scrH); // dof
	}
    renderer.bloomDofPass(pingpongFB, pingpongBloomTex, pingpongDofTex, bloomTex, dofTex, screenspaceTex);

    bindFramebuffer(0);
    renderer.doubleBlendPass(pingpongDofTex[!renderer.horizontal], pingpongBloomTex[!renderer.horizontal]);
}

void display() {
    // animate
    for (usize i = 0; i < MODELS_COUNT; i++)
        if (models[i].shape->bonesPerVertex != 0)
            models[i].animator->animate(glfwGetTime());

    /* --- shadow rendering --- */
    glUseProgram(ss.programId);
	for (GLuint i = 0; i < LAMPS_COUNT; i++) {
        glUniform1f(ss.far, lightSources[i].far);
        renderToDepthMap(i);
    }
	glUseProgram(0);

    glUseProgram(sss.programId);
    setMat4(sss.matProjection, projectionMatrix);
    setMat4(sss.matView, viewMatrix);

    glUseProgram(0);

	/* --- color rendering --- */
	glUseProgram(cs.programId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render();
	glUseProgram(0);
}

void animate_scene() {
    glm::mat3 rotate = glm::mat3(glm::rotate(glm::mat4(), glm::radians(0.01f), glm::vec3(0, 1, 0)));
    while (true) {
        std::this_thread::sleep_for (std::chrono::milliseconds(1));
        lightSources[0].setPos(lightSources[0].pos * rotate);
    }
}

// The MAIN function, from here we start the application and run the game loop
int main() {
    initGL();
    initShaders();
    initVAO();
    initMatrices();
    params.scrW = WIN_W;
    params.scrH = WIN_H;
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
            GLfloat *pixels = getTexImageCube(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, lightSources[0].depthCubemap, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, GL_DEPTH_COMPONENT);
            saveTexImage(pixels, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 1, io::getCurrentDir() + "/out/scr_" + std::to_string(i) + ".bmp", 3, false);
            delete[] pixels;
        }
        std::cout << "Depth map data saved\n";

        GLfloat *pixels = getTexImage2D(screenspaceTex, WIN_W, WIN_H, GL_RGB);
        saveTexImage(pixels, WIN_W, WIN_H, 3, io::getCurrentDir() + "/out/scr_screenspace.bmp", 3);
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

        GLfloat *pixels = getPixels(positionTex, (size_t) x, WIN_H - (size_t) y, 1, 1, GL_RGB);
        
        std::cout << "x: " << pixels[0] << "; y: " << pixels[1] << "; z: " << pixels[2] << "\n";
        
        useShaderProgram(cs.programId);
        glUniform1f(cs.cinematicDOFPlaneInFocus, pixels[2] == 0 ? FLT_EPSILON : pixels[2]);
        useShaderProgram(0);

        delete[] pixels;
    }
}