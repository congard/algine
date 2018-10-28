/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
 * @author congard
 * 
 * @version 1.1 alpha
 */

#include <iostream>
#include <thread>
#include <chrono>

#define ALGINE_LOGGING

#include "model.cpp"
#include "lamp.cpp"
#include "algine.cpp"
#include "sconstants.h"

#include "lib/libcmf/loader.cpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

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
glm::vec3 cameraPos(0, 6, 16);
glm::vec3 cameraLookAt(0, 4, 0);
glm::vec3 cameraUp(0, 1, 0);

// models
ModelBuffers mbuffers[MODEL_BUFFERS_MAPPING_COUNT];
ModelMapping mmapping[MODEL_BUFFERS_MAPPING_COUNT];
Model models[MODELS_COUNT], lamps[LAMPS_COUNT];

// light
LampModel lightSources[LAMPS_COUNT];

// shadow maps
TextureArray shadowMaps;

// renderer
AlgineRenderer *renderer;

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
    ALGINE_SCR_W = WIN_W;
    ALGINE_SCR_H = WIN_H;
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

    result.loadLocations(ALGINE_PROGRAM_ID_CS, id);
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
void create_ModelBuffers_ModelMapping(const char *path, const char *atex, const char *dtex, const char *stex, const char *ntex, size_t id, bool inverseNormals) {
    CMFLoader loader;
    loader.loadCMF(path);
    loader.load();

    if (inverseNormals) Model::inverse(loader.normals);

    CMF2Arrays c2a;
    c2a.init(&loader.vertices, &loader.normals, &loader.texCoords, &loader.faces);
    c2a.make(true, true, true);

    mbuffers[id].init(c2a);
    mmapping[id].init(atex, dtex, stex, ntex);

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
void create_ModelBuffers_ModelMapping(const char *path, const char *atex, const char *dtex, const char *stex, const char *ntex, size_t id) {
    create_ModelBuffers_ModelMapping(path, atex, dtex, stex, ntex, id, false);
}

/* init code begin */

/**
 * Init GL: creating window, etc.
 */
void initGL() {
    std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
    // Init GLFW
    glfwInit();
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
    // glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
    //glCullFace(GL_BACK);
}

/**
 * Loading and compiling shaders
 */
void initShaders() {
    ALGINE_PATHS_TO_SHADERS apts {
        (const char*) "src/resources/shaders/vertex_shader.glsl",
        (const char*) "src/resources/shaders/fragment_shader.glsl",

        (const char*) "src/resources/shaders/vertex_shadow_shader.glsl",
        (const char*) "src/resources/shaders/geometry_shadow_shader.glsl",
        (const char*) "src/resources/shaders/fragment_shadow_shader.glsl",

        (const char*) "src/resources/shaders/vertex_blur_shader.glsl",
        (const char*) "src/resources/shaders/fragment_blur_shader.glsl",

        (const char*) "src/resources/shaders/vertex_blend_shader.glsl",
        (const char*) "src/resources/shaders/fragment_blend_shader.glsl"
    };

    std::cout << "Compiling algine shaders\n";

    //ALGINE_TEXTURE_MAPPING_MODE = ALGINE_TEXTURE_MAPPING_MODE_DISABLED;
    //ALGINE_NORMAL_MAPPING_MODE = ALGINE_NORMAL_MAPPING_MODE_DISABLED;
    //ALGINE_LIGHTING_MODE = ALGINE_LIGHTING_MODE_DISABLED;
    //ALGINE_ATTENUATION_MODE = ALGINE_ATTENUATION_MODE_DISABLED;
    //ALGINE_DOF_MODE = ALGINE_DOF_MODE_DISABLED;
    //ALGINE_BLOOM_MODE = ALGINE_BLOOM_MODE_DISABLED;
    //ALGINE_SHADOW_MAPPING_MODE = ALGINE_SHADOW_MAPPING_MODE_DISABLED;
    //ALGINE_KERNEL_BLOOM_SIZE = 4;
    ALGINE_KERNEL_DOF_SIZE = 3;
    algine_compile_shaders(algine_create_shaders_sources(apts));
    algine_load_locations();

    renderer = new AlgineRenderer();
    renderer->prepare();
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
        0
    );

    // Brickwall
    create_ModelBuffers_ModelMapping(
        "src/resources/models/brickwall/brickwall_triangulated_big.cmf",
        "src/resources/textures/brickwall/brickwall_big_2k.png",
        "src/resources/textures/brickwall/brickwall_big_2k.png",
        "src/resources/textures/brickwall/brickwall_big_2k.png",
        "src/resources/textures/brickwall/brickwall_big_normal_2k.png",
        1
    );

    // Box
    create_ModelBuffers_ModelMapping(
        "src/resources/models/cube/cube_triangulated.cmf",
        "src/resources/textures/cube/cube_ambient.jpg",
        "src/resources/textures/cube/cube_diffuse.jpg",
        "src/resources/textures/cube/cube_specular.jpg",
        "src/resources/textures/cube/cube_normal.jpg",
        2
    );

    // Japanese lamp
    create_ModelBuffers_ModelMapping(
        "src/resources/models/japanese_lamp/japanese_lamp.cmf",
        "src/resources/textures/japanese_lamp/ambient.tga",
        "src/resources/textures/japanese_lamp/diffuse.tga",
        "src/resources/textures/japanese_lamp/specular.tga",
        "src/resources/textures/japanese_lamp/normal.tga",
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
    Lamp::lightPosLocation = ALGINE_SS_LAMP_POSITION;
    Lamp::initShadowMatrices(ALGINE_PROGRAM_ID_SS);

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
    shadowMaps.init(ALGINE_PROGRAM_ID_CS, ALGINE_NAME_CS_SHADOW_MAPS);
    for (size_t i = 0; i < LAMPS_COUNT; i++) shadowMaps.addTexture(i, lightSources[i].depthCubemap);
    shadowMaps.bind(ALGINE_PROGRAM_ID_CS);
}

void initShadowCalculation() {
    glUseProgram(ALGINE_PROGRAM_ID_CS);
    glUniform1f(ALGINE_CS_SHADOW_DISKRADIUS_K, diskRadius_k);
    glUniform1f(ALGINE_CS_SHADOW_DISKRADIUS_MIN, diskRadius_min);
    glUniform1f(ALGINE_CS_SHADOW_BIAS, shadow_bias);
    glUseProgram(0);
}

/**
 * Initialize Depth of field
 */
void initDOF() {
    glUseProgram(ALGINE_PROGRAM_ID_BLUS_HORIZONTAL);
    glUniform1f(ALGINE_BLUS_H_SIGMA_MAX, dof_max_sigma);
    glUniform1f(ALGINE_BLUS_H_SIGMA_MIN, dof_min_sigma);
    glUseProgram(ALGINE_PROGRAM_ID_BLUS_VERTICAL);
    glUniform1f(ALGINE_BLUS_V_SIGMA_MAX, dof_max_sigma);
    glUniform1f(ALGINE_BLUS_V_SIGMA_MIN, dof_min_sigma);
    glUseProgram(ALGINE_PROGRAM_ID_CS);
    glUniform1f(ALGINE_CS_FOCAL_DEPTH, focalDepth);
    glUniform1f(ALGINE_CS_FOCAL_RANGE, focalRange);
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
    delete renderer;
}

void sendLampsData() {
	glUniform1i(ALGINE_CS_LAMPS_COUNT, LAMPS_COUNT); // lamps count
	su::setVec3(ALGINE_CS_VIEW_POSITION, cameraPos); // current camera position
	glUniform1f(ALGINE_CS_SHADOW_FAR_PLANE, LAMP_FAR_PLANE); // far plane in shadow matrices
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
    su::setMat4(ALGINE_CS_MAT_PVM, getPVMMatrix());
    su::setMat4(ALGINE_CS_MAT_VM, getVMMatrix());
    su::setMat4(ALGINE_CS_MAT_MODEL, modelMatrix);
    su::setMat4(ALGINE_CS_MAT_VIEW, viewMatrix);
}
/* --- --- */

#define polygons buffers->polygons

/**
 * Draws model in depth map
 * @param shaderProgram
 * @param model
 */
void drawModelDM(Model &model) {
	su::pointer(ALGINE_SS_IN_POSITION, 3, model.getVerticesBuffer());
	modelMatrix = glm::mat4();
    modelMatrix = glm::translate(modelMatrix, model.origin);
    modelMatrix = glm::rotate(modelMatrix, model.angles[0], glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, model.angles[1], glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, model.angles[2], glm::vec3(0, 0, 1));
	su::setMat4(ALGINE_SS_MAT_MODEL, modelMatrix);
	for (size_t i = 0; i < model.polygons.size; i++) glDrawArrays(model.polygons.array[i].type, model.polygons.array[i].start, model.polygons.array[i].count); // drawing
}

#define ambientTexture mapping->textures[0]
#define diffuseTexture mapping->textures[1]
#define specularTexture mapping->textures[2]
#define normalTexture mapping->textures[3]

/**
 * Draws models from ModelObject 2D array
 * @param gl
 * @param shaderProgram
 * @param modelsArray
 */
void drawModel(Model &model) {
	bindADSNTextures(
        ALGINE_CS_SAMPLER_MAPPING_AMBIENT, ALGINE_CS_SAMPLER_MAPPING_DIFFUSE, ALGINE_CS_SAMPLER_MAPPING_SPECULAR, ALGINE_CS_SAMPLER_MAPPING_NORMAL,
        model.ambientTexture, model.diffuseTexture, model.specularTexture, model.normalTexture
    );
    
	su::pointer(ALGINE_CS_IN_POSITION, 3, model.getVerticesBuffer());
	su::pointer(ALGINE_CS_IN_NORMAL, 3, model.getNormalsBuffer());
	su::pointer(ALGINE_CS_IN_TANGENT, 3, model.getTangentsBuffer());
	su::pointer(ALGINE_CS_IN_BITANGENT, 3, model.getBitangentsBuffer());
	su::pointer(ALGINE_CS_IN_TEXCOORD, 2, model.getTexCoordsBuffer());
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
		
	glEnableVertexAttribArray(ALGINE_SS_IN_POSITION);
		
	// drawing models
	for (size_t i = 0; i < MODELS_COUNT; i++) drawModelDM(models[i]);
		
	// drawing lamps
	for (GLuint i = 0; i < LAMPS_COUNT; i++) {
		if (i == index) continue;
		drawModelDM(lamps[i]);
	}
		
	glDisableVertexAttribArray(ALGINE_SS_IN_POSITION);
	
	lightSources[index].end();
}

/**
 * Color rendering
 */

#define begin (renderer->*(renderer->begin_rendering))()
#define end (renderer->*(renderer->end_rendering))()

void render() {
    // bloom, dof, blending
    begin;

	// view port to window size
	glViewport(0, 0, WIN_W, WIN_H);
	// updating view matrix (because camera position was changed)
	createViewMatrix();
	// sending lamps parameters to fragment shader
	sendLampsData();
	
    glEnableVertexAttribArray(ALGINE_CS_IN_POSITION);
    glEnableVertexAttribArray(ALGINE_CS_IN_NORMAL);
    glEnableVertexAttribArray(ALGINE_CS_IN_TEXCOORD);
	
	// drawing
	//glUniform1f(ALGINE_CS_SWITCH_NORMAL_MAPPING, 1); // with mapping
    glEnableVertexAttribArray(ALGINE_CS_IN_TANGENT);
    glEnableVertexAttribArray(ALGINE_CS_IN_BITANGENT);
	
    for (size_t i = 0; i < MODELS_COUNT; i++) drawModel(models[i]);
	for (size_t i = 0; i < LAMPS_COUNT; i++) drawModel(lamps[i]);
	
    glDisableVertexAttribArray(ALGINE_CS_IN_POSITION);
    glDisableVertexAttribArray(ALGINE_CS_IN_NORMAL);
    glDisableVertexAttribArray(ALGINE_CS_IN_TEXCOORD);
    glDisableVertexAttribArray(ALGINE_CS_IN_TANGENT);
    glDisableVertexAttribArray(ALGINE_CS_IN_BITANGENT);

    end;
}

#undef begin
#undef end

void display() {
    /* --- shadow rendering --- */
    glUseProgram(ALGINE_PROGRAM_ID_SS);
	glUniform1f(ALGINE_SS_FAR_PLANE, LAMP_FAR_PLANE);
	for (GLuint i = 0; i < LAMPS_COUNT; i++) renderToDepthMap(i);
	glUseProgram(0);
		
	/* --- color rendering --- */
	glUseProgram(ALGINE_PROGRAM_ID_CS);
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
    ALGINE_SCR_W = WIN_W;
    ALGINE_SCR_H = WIN_H;
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
        glUseProgram(ALGINE_PROGRAM_ID_CS);

        if (key == GLFW_KEY_Q) {
            focalDepth++;
            glUniform1f(ALGINE_CS_FOCAL_DEPTH, focalDepth);
        } else if (key == GLFW_KEY_A) {
            focalDepth--;
            glUniform1f(ALGINE_CS_FOCAL_DEPTH, focalDepth);
        } else if (key == GLFW_KEY_W) {
            focalRange += 5;
            glUniform1f(ALGINE_CS_FOCAL_RANGE, focalRange);
        } else if (key == GLFW_KEY_S) {
            focalRange -= 5;
            glUniform1f(ALGINE_CS_FOCAL_RANGE, focalRange);
        }

        glUseProgram(0);

        std::cout << "focalDepth = " << focalDepth << "\nfocalRange = " << focalRange << "\n";
    }
}