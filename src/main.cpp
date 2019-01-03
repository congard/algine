/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
 * @author congard
 * 
 * @version 1.3 beta-candidate
 */

#include <iostream>
#include <thread>
#include <chrono>

#define ALGINE_LOGGING

#include "algine.cpp"
#include "lib/libcmf/loader.cpp"

#define SHADOW_MAP_RESOLUTION 1024

#define FULLSCREEN !true

#define LAMPS_COUNT 1
/**
 * Model Buffers amd Model Mapping on this scene:
 * 1. Lucy
 * 2. Brickwall
 * 3. Box
 * 4. Japanese lamp
 */
#define MODEL_BUFFERS_MAPPING_COUNT 4
/**
 * Models on this scene:
 * 1. Lucy
 * 2. Brickwall
 * 3. Box ( 4,  0,  8)
 * 4. Box (-4,  0,  8)
 * 5. Box ( 4,  0, -8)
 * 6. Box (-4,  0, -8)
 * All 4 boxes use 1 ModelBuffers and 1 ModelMapping
 */
#define MODELS_COUNT 6

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
GLuint WIN_W = 1366, WIN_H = 763;
GLFWwindow* window;

GLuint vao;

// matrices
glm::mat4 projectionMatrix, viewMatrix, modelMatrix;

// camera params
glm::vec3 cameraPos(0, 6, 14); // prev: 10; 6; 8
glm::vec3 cameraLookAt(0, 4, 0);
glm::vec3 cameraUp(0, 1, 0);

// framebuffers, textures etc for rendering
using namespace algine;

// models
ModelBuffers mbuffers[MODEL_BUFFERS_MAPPING_COUNT];
ModelMapping mmapping[MODEL_BUFFERS_MAPPING_COUNT];
Model models[MODELS_COUNT], lamps[LAMPS_COUNT];

// light
LampModel lightSources[LAMPS_COUNT];

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

AlginePrograms programs;
AlgineParams params;

float
    // DOF variables
    focalDepth = 8.0f, 
    focalRange = 20.0f,
    dof_max_sigma = 6.0f,
    dof_min_sigma = 0.0001f,
    
    // diskRadius variables
    diskRadius_k = 1.0f / 25.0f,
    diskRadius_min = 0.0f,
    
    // other shadow variables
    shadow_bias = 0.3f;

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
    params.SCR_W = WIN_W;
    params.SCR_H = WIN_H;
    createProjectionMatrix();
}

/**
 * Creates Lamp with default params
 */
void createLamp(LampModel &result, const glm::vec3 &pos, const glm::vec3 &color, GLuint id) {
    result.pos = pos;
    result.color = color;
    result.ambientStrength = 0.01f;
    result.diffuseStrength = 1.0f;
    result.specularStrength = 1.0f;
    result.kc = 1;
    result.kl = 0.045f;
    result.kq = 0.0075f;
    result.shininess = 32;

    result.loadLocations(programs.PROGRAM_ID_CS, id);
    result.initShadowMapping(SHADOW_MAP_RESOLUTION);
}

/**
 * Creates ModelBuffers and ModelMapping
 * @param *path - path to cmf file
 * @param *atex - path to ambient texture
 * @param *dtex - path to diffuse texture
 * @param *stex - path to specular texture
 * @param *ntex - path to normal texture
 * @param id - ModelXXX id
 * @param inverseNormals - if true, normals will be inverted
 */
void create_ModelBuffers_ModelMapping(const char *path, const char *atex, const char *dtex, const char *stex, const char *ntex, const char *rstex, const char *jtex, size_t id, bool inverseNormals) {
    CMFLoader loader;
    loader.loadCMF(path);
    loader.load();

    if (inverseNormals) Model::inverse(loader.normals);

    CMF2Arrays c2a;
    c2a.init(&loader.vertices, &loader.normals, &loader.texCoords, &loader.faces);
    c2a.make(true, true, true);

    mbuffers[id].init(c2a);
    mmapping[id].init(atex, dtex, stex, ntex, rstex, jtex);

    c2a.cleanup();
}

/**
 * Creates ModelBuffers and ModelMapping
 * @param *path - path to cmf file
 * @param *atex - path to ambient texture
 * @param *dtex - path to diffuse texture
 * @param *stex - path to specular texture
 * @param *ntex - path to normal texture
 * @param id - ModelXXX id
 * inverseNormals set to false
 */
void create_ModelBuffers_ModelMapping(const char *path, const char *atex, const char *dtex, const char *stex, const char *ntex, const char *rstex, const char *jtex, size_t id) {
    create_ModelBuffers_ModelMapping(path, atex, dtex, stex, ntex, rstex, jtex, id, false);
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

    params.KERNEL_DOF_SIZE = 3;
    programs.PROGRAM_ID_CS = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getCS(
        params,
        "src/resources/shaders/vertex_shader.glsl",
        "src/resources/shaders/fragment_shader.glsl")
    ));
    programs.PROGRAM_ID_SS = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getSS(
        "src/resources/shaders/vertex_shadow_shader.glsl",
        "src/resources/shaders/fragment_shadow_shader.glsl",
        "src/resources/shaders/geometry_shadow_shader.glsl")
    ));
    programs.PROGRAM_ID_SSS = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getSSS(
        params,
        "src/resources/shaders/vertex_screenspace_shader.glsl",
        "src/resources/shaders/fragment_screenspace_shader.glsl")
    ));
    std::vector<ShadersData> blus = scompiler::getBLUS(
        params,
        "src/resources/shaders/vertex_blur_shader.glsl",
        "src/resources/shaders/fragment_blur_shader.glsl"
    );
    programs.PROGRAM_ID_BLUS_HORIZONTAL = scompiler::createShaderProgramDS(scompiler::compileShaders(
        blus[0]
    ));
    programs.PROGRAM_ID_BLUS_VERTICAL = scompiler::createShaderProgramDS(scompiler::compileShaders(
        blus[1]
    ));
    programs.PROGRAM_ID_BLES = scompiler::createShaderProgramDS(scompiler::compileShaders(scompiler::getBLES(
        params,
        "src/resources/shaders/vertex_blend_shader.glsl",
        "src/resources/shaders/fragment_blend_shader.glsl")
    ));
    scompiler::loadLocations(programs, params);

    renderer.programs = programs;
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

    GLuint displayColorAttachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    bindFramebuffer(displayFB);
    glDrawBuffers(5, displayColorAttachments);

    Framebuffer::attachTexture(colorTex, COLOR_ATTACHMENT(0));
    Framebuffer::attachTexture(dofTex, COLOR_ATTACHMENT(1));
    Framebuffer::attachTexture(normalTex, COLOR_ATTACHMENT(2));
    Framebuffer::attachTexture(ssrValues, COLOR_ATTACHMENT(3));
    Framebuffer::attachTexture(positionTex, COLOR_ATTACHMENT(4));

    GLuint colorAttachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

    for (size_t i = 0; i < 2; i++) {
        // configuring ping-pong (blur) textures
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFB[i]);
        glDrawBuffers(2, colorAttachments); // use 2 color components (boom / dof)

        Framebuffer::attachTexture(pingpongBloomTex[i], COLOR_ATTACHMENT(0));
        Framebuffer::attachTexture(pingpongDofTex[i], COLOR_ATTACHMENT(1));
    }

    // sending to shader center of kernel and right part
    float kernel[(params.KERNEL_BLOOM_SIZE - 1) * 2];
    getGB1DKernel(kernel, (params.KERNEL_BLOOM_SIZE - 1) * 2, params.KERNEL_BLOOM_SIGMA);

    for (size_t j = 0; j < 2; j++) {
        glUseProgram(renderer.blusPrograms[j].programId);
        for (GLuint i = 0; i < params.KERNEL_BLOOM_SIZE; i++)
            glUniform1f(
                j ? programs.BLUS_H_KERNEL_BLOOM[params.KERNEL_BLOOM_SIZE - 1 - i] : programs.BLUS_V_KERNEL_BLOOM[params.KERNEL_BLOOM_SIZE - 1 - i],
                kernel[i]);
    }

    glUseProgram(0);

    bindFramebuffer(screenspaceFB);
    glDrawBuffers(2, colorAttachments);
    Framebuffer::attachTexture(screenspaceTex, COLOR_ATTACHMENT(0));
    Framebuffer::attachTexture(bloomTex, COLOR_ATTACHMENT(1));

    bindFramebuffer(0);

    // configuring CS
    useShaderProgram(programs.PROGRAM_ID_CS);
    glUniform1i(programs.CS_SAMPLER_MAPPING_AMBIENT, 0);
    glUniform1i(programs.CS_SAMPLER_MAPPING_DIFFUSE, 1);
    glUniform1i(programs.CS_SAMPLER_MAPPING_SPECULAR, 2);
    glUniform1i(programs.CS_SAMPLER_MAPPING_NORMAL, 3);
    glUniform1i(programs.CS_SAMPLER_MAPPING_REFLECTIONSTRENGTH, 4);
    glUniform1i(programs.CS_SAMPLER_MAPPING_JITTER, 5);
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
 * Creating buffers and loading textures
 */
void init_ModelBuffers_ModelMapping() {
    // Lucy
    create_ModelBuffers_ModelMapping(
        "src/resources/models/LucyAngel/Stanfords_Lucy_Angel_triangulated.cmf",
        "src/resources/textures/LucyAngel/Stanfords_Lucy_Angel_diffuse_1k.jpg",
        "src/resources/textures/LucyAngel/Stanfords_Lucy_Angel_diffuse_1k.jpg",
        "src/resources/textures/LucyAngel/Stanfords_Lucy_Angel_specular_1k.jpg",
        "src/resources/textures/LucyAngel/Stanfords_Lucy_Angel_normal_1k.jpg",
        nullptr, nullptr,
        0
    );

    // Brickwall
    create_ModelBuffers_ModelMapping(
        "src/resources/models/brickwall/brickwall_triangulated_big.cmf",
        nullptr, nullptr, nullptr,
        "src/resources/textures/brickwall/brickwall_big_normal_2k.png",
        "src/resources/textures/brickwall/brickwall_big_reflection_strength_2k.png",
        "src/resources/textures/brickwall/brickwall_big_jitter_2k.png",
        1
    );

    // Box
    create_ModelBuffers_ModelMapping(
        "src/resources/models/cube/cube_triangulated.cmf",
        "src/resources/textures/cube/cube_ambient.jpg",
        "src/resources/textures/cube/cube_diffuse.jpg",
        "src/resources/textures/cube/cube_specular.jpg",
        "src/resources/textures/cube/cube_normal.jpg",
        "src/resources/textures/cube/cube_reflection_strength.jpg",
        "src/resources/textures/cube/cube_jitter.jpg",
        2
    );

    // Japanese lamp
    create_ModelBuffers_ModelMapping(
        "src/resources/models/japanese_lamp/japanese_lamp.cmf",
        "src/resources/textures/japanese_lamp/ambient.tga",
        "src/resources/textures/japanese_lamp/diffuse.tga",
        "src/resources/textures/japanese_lamp/specular.tga",
        "src/resources/textures/japanese_lamp/normal.tga",
        nullptr, nullptr,
        3, true
    );
}

/**
 * Creating models from created buffers and loaded textures
 */
void createModels() {
    // lucy
    models[0].buffers = &mbuffers[0];
    models[0].mapping = &mmapping[0];
    //models[0].origin = glm::vec3(4, 0, 0);
    
    // brickwall
    models[1].buffers = &mbuffers[1];
    models[1].mapping = &mmapping[1];
    models[1].angles = glm::vec3(glm::radians(270.0f), 0, 0);

    // boxes
    models[2].buffers = &mbuffers[2];
    models[2].mapping = &mmapping[2];
    models[2].origin = glm::vec3(4, 0, 8);

    models[3] = models[2];
    models[3].origin = glm::vec3(-4, 0, 8);

    models[4] = models[2];
    models[4].origin = glm::vec3(4, 0, -8);

    models[5] = models[2];
    models[5].origin = glm::vec3(-4, 0, -8);
}

/**
 * Creating light sources
 */
void initLamps() {
    Lamp::lightPosLocation = programs.SS_LAMP_POSITION;
    Lamp::initShadowMatrices(programs.PROGRAM_ID_SS);

    createLamp(lightSources[0], glm::vec3(8.0f, 8.0f, -2.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0);
    lamps[0].buffers = &mbuffers[3];
    lamps[0].mapping = &mmapping[3];
    lightSources[0].mptr = &lamps[0];

    // // second lamp
    // createLamp(lightSources[1], glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1);
    // lamps[1].buffers = &mbuffers[3];
    // lamps[1].mapping = &mmapping[3];
    // lamps[1].origin = lightSources[1].pos;
    // lightSources[1].mptr = &lamps[1];
}

/**
 * Binds to depth cubemaps
 */
void initShadowMaps() {
    shadowMaps = TextureArray(10, LAMPS_COUNT);
    shadowMaps.init(programs.PROGRAM_ID_CS, ALGINE_NAME_CS_SHADOW_MAPS);
    for (size_t i = 0; i < LAMPS_COUNT; i++) shadowMaps.addTexture(i, lightSources[i].depthCubemap);
    shadowMaps.bind(programs.PROGRAM_ID_CS);
}

void initShadowCalculation() {
    glUseProgram(programs.PROGRAM_ID_CS);
    glUniform1f(programs.CS_SHADOW_DISKRADIUS_K, diskRadius_k);
    glUniform1f(programs.CS_SHADOW_DISKRADIUS_MIN, diskRadius_min);
    glUniform1f(programs.CS_SHADOW_BIAS, shadow_bias);
    glUseProgram(0);
}

/**
 * Initialize Depth of field
 */
void initDOF() {
    glUseProgram(programs.PROGRAM_ID_BLUS_HORIZONTAL);
    glUniform1f(programs.BLUS_H_SIGMA_MAX, dof_max_sigma);
    glUniform1f(programs.BLUS_H_SIGMA_MIN, dof_min_sigma);
    glUseProgram(programs.PROGRAM_ID_BLUS_VERTICAL);
    glUniform1f(programs.BLUS_V_SIGMA_MAX, dof_max_sigma);
    glUniform1f(programs.BLUS_V_SIGMA_MIN, dof_min_sigma);
    glUseProgram(programs.PROGRAM_ID_CS);
    glUniform1f(programs.CS_FOCAL_DEPTH, focalDepth);
    glUniform1f(programs.CS_FOCAL_RANGE, focalRange);
    glUseProgram(0);
}

/* init code end */

/**
 * Cleans memory before exit
 */
void recycleAll() {
    glDeleteVertexArrays(1, &vao);
    for (size_t i = 0; i < MODEL_BUFFERS_MAPPING_COUNT; i++) {
        mbuffers[i].recycle();
        mmapping[i].recycle();
    }
    //delete renderer;
}

void sendLampsData() {
	glUniform1i(programs.CS_LAMPS_COUNT, LAMPS_COUNT); // lamps count
	setVec3(programs.CS_VIEW_POSITION, cameraPos); // current camera position
	glUniform1f(programs.CS_SHADOW_FAR_PLANE, LAMP_FAR_PLANE); // far plane in shadow matrices
	for (size_t i = 0; i < LAMPS_COUNT; i++) lightSources[i].pushAll();
}

/* --- matrices --- */
glm::mat4 getPVMMatrix() {
    return projectionMatrix * viewMatrix * modelMatrix;
}

glm::mat4 getVMMatrix() {
    return viewMatrix * modelMatrix;
}

void updateMatrices() {
    setMat4(programs.CS_MAT_PVM, getPVMMatrix());
    setMat4(programs.CS_MAT_VM, getVMMatrix());
    setMat4(programs.CS_MAT_MODEL, modelMatrix);
    setMat4(programs.CS_MAT_VIEW, viewMatrix);
}
/* --- --- */

#define polygons buffers->polygons

/**
 * Draws model in depth map
 * @param shaderProgram
 * @param model
 */
void drawModelDM(Model &model) {
	pointer(programs.SS_IN_POSITION, 3, model.getVerticesBuffer());
	modelMatrix = glm::mat4();
    modelMatrix = glm::translate(modelMatrix, model.origin);
    modelMatrix = glm::rotate(modelMatrix, model.angles[0], glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, model.angles[1], glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, model.angles[2], glm::vec3(0, 0, 1));
	setMat4(programs.SS_MAT_MODEL, modelMatrix);
	for (size_t i = 0; i < model.polygons.size; i++) glDrawArrays(model.polygons.array[i].type, model.polygons.array[i].start, model.polygons.array[i].count); // drawing
}

#define ambientTexture mapping->textures[0]
#define diffuseTexture mapping->textures[1]
#define specularTexture mapping->textures[2]
#define normalTexture mapping->textures[3]
#define reflectionStrengthTexture mapping->textures[4]
#define jitterTexture mapping->textures[5]

/**
 * Draws model
 */
void drawModel(Model &model) {
    texture2DAB(0, model.ambientTexture);
    texture2DAB(1, model.diffuseTexture);
    texture2DAB(2, model.specularTexture);
    texture2DAB(3, model.normalTexture);
    texture2DAB(4, model.reflectionStrengthTexture);
    texture2DAB(5, model.jitterTexture);
    
	pointer(programs.CS_IN_POSITION, 3, model.getVerticesBuffer());
	pointer(programs.CS_IN_NORMAL, 3, model.getNormalsBuffer());
	pointer(programs.CS_IN_TANGENT, 3, model.getTangentsBuffer());
	pointer(programs.CS_IN_BITANGENT, 3, model.getBitangentsBuffer());
	pointer(programs.CS_IN_TEXCOORD, 2, model.getTexCoordsBuffer());
	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, model.origin);
    modelMatrix = glm::rotate(modelMatrix, model.angles[0], glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, model.angles[1], glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, model.angles[2], glm::vec3(0, 0, 1));
	updateMatrices();
	for (size_t i = 0; i < model.polygons.size; i++) glDrawArrays(model.polygons.array[i].type, model.polygons.array[i].start, model.polygons.array[i].count); // drawing
}

#undef ambientTexture
#undef diffuseTexture
#undef specularTexture
#undef normalTexture
#undef polygons

/**
 * Renders to depth cubemap
 */
void renderToDepthMap(GLuint index) {
	lightSources[index].begin();
	lightSources[index].setLightPos();
	lightSources[index].updateMatrices();
	lightSources[index].setShadowMatrices();
	glClear(GL_DEPTH_BUFFER_BIT);
		
	glEnableVertexAttribArray(programs.SS_IN_POSITION);
		
	// drawing models
	for (size_t i = 0; i < MODELS_COUNT; i++) drawModelDM(models[i]);
		
	// drawing lamps
	for (GLuint i = 0; i < LAMPS_COUNT; i++) {
		if (i == index) continue;
		drawModelDM(lamps[i]);
	}
		
	glDisableVertexAttribArray(programs.SS_IN_POSITION);
	
	lightSources[index].end();
}

/**
 * Color rendering
 */
void render() {
    renderer.mainPass(displayFB, rbo);
    cfgtex(colorTex, GL_RGBA16F, GL_RGBA, params.SCR_W, params.SCR_H);
    cfgtex(dofTex, GL_R16F, GL_RED, params.SCR_W, params.SCR_H);
    cfgtex(normalTex, GL_RGB16F, GL_RGB, params.SCR_W, params.SCR_H);
    cfgtex(ssrValues, GL_RG16F, GL_RG, params.SCR_W, params.SCR_H);
    cfgtex(positionTex, GL_RGB16F, GL_RGB, params.SCR_W, params.SCR_H);
    glClearBufferfv(GL_COLOR, 1, ALGINE_RED); // dof buffer

	// view port to window size
	glViewport(0, 0, WIN_W, WIN_H);
	// updating view matrix (because camera position was changed)
	createViewMatrix();
	// sending lamps parameters to fragment shader
	sendLampsData();
    glEnableVertexAttribArray(programs.CS_IN_POSITION);
    glEnableVertexAttribArray(programs.CS_IN_NORMAL);
    glEnableVertexAttribArray(programs.CS_IN_TEXCOORD);
	
	// drawing
	//glUniform1f(ALGINE_CS_SWITCH_NORMAL_MAPPING, 1); // with mapping
    glEnableVertexAttribArray(programs.CS_IN_TANGENT);
    glEnableVertexAttribArray(programs.CS_IN_BITANGENT);
	
    for (size_t i = 0; i < MODELS_COUNT; i++) drawModel(models[i]);
	for (size_t i = 0; i < LAMPS_COUNT; i++) drawModel(lamps[i]);
	
    glDisableVertexAttribArray(programs.CS_IN_POSITION);
    glDisableVertexAttribArray(programs.CS_IN_NORMAL);
    glDisableVertexAttribArray(programs.CS_IN_TEXCOORD);
    glDisableVertexAttribArray(programs.CS_IN_TANGENT);
    glDisableVertexAttribArray(programs.CS_IN_BITANGENT);

    cfgtex(screenspaceTex, GL_RGB16F, GL_RGB, params.SCR_W, params.SCR_H);
    cfgtex(bloomTex, GL_RGB16F, GL_RGB, params.SCR_W, params.SCR_H);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    renderer.screenspacePass(screenspaceFB, colorTex, normalTex, ssrValues, positionTex);

    // configuring textures
	for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFB[i]);
        cfgtex(pingpongBloomTex[i], GL_RGB16F, GL_RGB, params.SCR_W, params.SCR_H); // bloom
		cfgtex(pingpongDofTex[i], GL_RGB16F, GL_RGB, params.SCR_W, params.SCR_H); // dof
	}
    renderer.bloomDofPass(pingpongFB, pingpongBloomTex, pingpongDofTex, bloomTex, dofTex, screenspaceTex);
    
    bindFramebuffer(0);
    renderer.doubleBlendPass(pingpongDofTex[!renderer.horizontal], pingpongBloomTex[!renderer.horizontal]);
}

void display() {
    /* --- shadow rendering --- */
    glUseProgram(programs.PROGRAM_ID_SS);
	glUniform1f(programs.SS_FAR_PLANE, LAMP_FAR_PLANE);
	for (GLuint i = 0; i < LAMPS_COUNT; i++) renderToDepthMap(i);
	glUseProgram(0);

    glUseProgram(programs.PROGRAM_ID_SSS);
    setMat4(programs.SSS_MAT_PROJECTION, projectionMatrix);
    setMat4(programs.SSS_MAT_VIEW, viewMatrix);

    glUseProgram(0);
		
	/* --- color rendering --- */
	glUseProgram(programs.PROGRAM_ID_CS);
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
    params.SCR_W = WIN_W;
    params.SCR_H = WIN_H;
    init_ModelBuffers_ModelMapping();
    createModels();
    initLamps();
    initShadowMaps();
    initShadowCalculation();
    initDOF();

    std::thread animate_scene_th(animate_scene);

    double currentTime, previousTime = glfwGetTime();
    size_t frameCount = 0;
    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        frameCount++;
        // If a second has passed.
        if (currentTime - previousTime >= 1.0) {
            // Display the frame count here any way you want.
            std::cout << frameCount << " fps\n";
            frameCount = 0;
            previousTime = currentTime;
        }

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        display();
        glfwSwapBuffers(window);
    }

    animate_scene_th.detach();

    recycleAll();

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
    else if ((key == GLFW_KEY_Q || key == GLFW_KEY_A || key == GLFW_KEY_W || key == GLFW_KEY_S) && action == GLFW_PRESS) {
        glUseProgram(programs.PROGRAM_ID_CS);

        if (key == GLFW_KEY_Q) {
            focalDepth++;
            glUniform1f(programs.CS_FOCAL_DEPTH, focalDepth);
        } else if (key == GLFW_KEY_A) {
            focalDepth--;
            glUniform1f(programs.CS_FOCAL_DEPTH, focalDepth);
        } else if (key == GLFW_KEY_W) {
            focalRange += 5;
            glUniform1f(programs.CS_FOCAL_RANGE, focalRange);
        } else if (key == GLFW_KEY_S) {
            focalRange -= 5;
            glUniform1f(programs.CS_FOCAL_RANGE, focalRange);
        }

        glUseProgram(0);

        std::cout << "focalDepth = " << focalDepth << "\nfocalRange = " << focalRange << "\n";
    }
}