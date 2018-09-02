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

#include "utils/shader_utils.cpp"
#include "utils/texture_utils.cpp"
#include "model.cpp"
#include "lamp.cpp"
#include "advanced_rendering.cpp"
#include "utils/math.cpp"

#include "lib/libcmf/loader.cpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

// shaders
const char *vertexShader = "src/resources/shaders/vertex_shader.glsl";
const char *fragmentShader = "src/resources/shaders/fragment_shader.glsl";
const char *vertexShadowShader = "src/resources/shaders/vertex_shadow_shader.glsl";
const char *fragmentShadowShader = "src/resources/shaders/fragment_shadow_shader.glsl";
const char *geometryShadowShader = "src/resources/shaders/geometry_shadow_shader.glsl";

// shaders programs
ShaderProgram shaderProgram, shaderShadowProgram;

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

// ambient, diffuse, specular, normal mapping
Mapping mapping;

// DOF variables
float 
    focalDepth = 8.0f, 
    focalRange = 20.0f,
    dof_max_sigma = 6.0f,
    dof_min_sigma = 0.0001f;

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
    advren::SCR_W = WIN_W;
    advren::SCR_H = WIN_H;
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

    result.loadLocations(shaderProgram.programId, id);
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
    shaderProgram.init(vertexShader, fragmentShader);
    shaderProgram.loadValueId("a_Position", GLSL_ATTRIBUTE);
	shaderProgram.loadValueId("a_Normal", GLSL_ATTRIBUTE);
	shaderProgram.loadValueId("a_Texture", GLSL_ATTRIBUTE);
	shaderProgram.loadValueId("a_Tangent", GLSL_ATTRIBUTE);
	shaderProgram.loadValueId("a_Bitangent", GLSL_ATTRIBUTE);
	shaderProgram.loadValueId("u_LampsCount", GLSL_UNIFORM);
	shaderProgram.loadValueId("u_ViewPos", GLSL_UNIFORM);
	shaderProgram.loadValueId("shadowMaps", GLSL_UNIFORM);
	shaderProgram.loadValueId("u_Model", GLSL_UNIFORM);
	shaderProgram.loadValueId("u_View", GLSL_UNIFORM);
	shaderProgram.loadValueId("u_Projection", GLSL_UNIFORM);
	shaderProgram.loadValueId("far_plane", GLSL_UNIFORM);
    shaderProgram.loadValueId("focalDepth", GLSL_UNIFORM);
    shaderProgram.loadValueId("focalRange", GLSL_UNIFORM);

    shaderShadowProgram.init(vertexShadowShader, fragmentShadowShader, geometryShadowShader);
    shaderShadowProgram.loadValueId("a_Position", GLSL_ATTRIBUTE);
	shaderShadowProgram.loadValueId("u_ModelMatrix", GLSL_UNIFORM);
	shaderShadowProgram.loadValueId("lightPos", GLSL_UNIFORM);
	shaderShadowProgram.loadValueId("far_plane", GLSL_UNIFORM);
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
        "src/resources/textures/LucyAngel/Stanfords_Lucy_Angel_diffuse.jpg",
        "src/resources/textures/LucyAngel/Stanfords_Lucy_Angel_diffuse.jpg",
        "src/resources/textures/LucyAngel/Stanfords_Lucy_Angel_specular.jpg",
        "src/resources/textures/LucyAngel/Stanfords_Lucy_Angel_normal.jpg",
        0
    );

    // Brickwall
    create_ModelBuffers_ModelMapping(
        "src/resources/models/brickwall/brickwall_triangulated_big.cmf",
        "src/resources/textures/brickwall/brickwall_big.png",
        "src/resources/textures/brickwall/brickwall_big.png",
        "src/resources/textures/brickwall/brickwall_big.png",
        "src/resources/textures/brickwall/brickwall_big_normal.png",
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
    Lamp::lightPosLocation = shaderShadowProgram.getValueId("lightPos");
    Lamp::initShadowMatrices(shaderShadowProgram.programId);

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
    shadowMaps.init(shaderProgram.programId, "shadowMaps");
    for (size_t i = 0; i < LAMPS_COUNT; i++) shadowMaps.addTexture(i, lightSources[i].depthCubemap);
    shadowMaps.bind(shaderProgram.programId);
}

/**
 * Initialize Depth of field
 */
void initDOF() {
    glUseProgram(advren::shaderBlurProgram.programId);
    glUniform1f(advren::shaderBlurProgram.getValueId("max_sigma"), dof_max_sigma);
    glUniform1f(advren::shaderBlurProgram.getValueId("min_sigma"), dof_min_sigma);
    glUseProgram(shaderProgram.programId);
    glUniform1f(shaderProgram.getValueId("focalDepth"), focalDepth);
    glUniform1f(shaderProgram.getValueId("focalRange"), focalRange);
    glUseProgram(0);
}

/* init code end */

/**
 * Cleans memory before exit
 */
void recycleAll() {
    glDeleteVertexArrays(1, &vao);
    advren::recycle();
    for (size_t i = 0; i < MODEL_BUFFERS_MAPPING_COUNT; i++) {
        mbuffers[i].recycle();
        mmapping[i].recycle();
    }
}

void sendLampsData() {
	glUniform1i(shaderProgram.getValueId("u_LampsCount"), LAMPS_COUNT); // lamps count
	su::setVec3(shaderProgram.getValueId("u_ViewPos"), cameraPos); // current camera position
	glUniform1f(shaderProgram.getValueId("far_plane"), LAMP_FAR_PLANE); // far plane in shadow matrices
	for (size_t i = 0; i < LAMPS_COUNT; i++) lightSources[i].pushAll();
}

#define polygons buffers->polygons

/**
 * Draws model in depth map
 * @param shaderProgram
 * @param model
 */
void drawModelDM(ShaderProgram &shaderProgram, Model &model) {
	su::pointer(shaderProgram.getValueId("a_Position"), 3, model.getVerticesBuffer());
	modelMatrix = glm::mat4();
    modelMatrix = glm::translate(modelMatrix, model.origin);
    modelMatrix = glm::rotate(modelMatrix, model.angles[0], glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, model.angles[1], glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, model.angles[2], glm::vec3(0, 0, 1));
	su::setMat4(shaderProgram.getValueId("u_ModelMatrix"), modelMatrix);
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
void drawModel(ShaderProgram &shaderProgram, Model &model) {
	mapping.bindADSNTextures(model.ambientTexture, model.diffuseTexture, model.specularTexture, model.normalTexture);
	su::pointer(shaderProgram.getValueId("a_Position"), 3, model.getVerticesBuffer());
	su::pointer(shaderProgram.getValueId("a_Normal"), 3, model.getNormalsBuffer());
	su::pointer(shaderProgram.getValueId("a_Tangent"), 3, model.getTangentsBuffer());
	su::pointer(shaderProgram.getValueId("a_Bitangent"), 3, model.getBitangentsBuffer());
	su::pointer(shaderProgram.getValueId("a_Texture"), 2, model.getTexCoordsBuffer());
	modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, model.origin);
    modelMatrix = glm::rotate(modelMatrix, model.angles[0], glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, model.angles[1], glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, model.angles[2], glm::vec3(0, 0, 1));
	su::setMat4(shaderProgram.getValueId("u_Model"), modelMatrix);
	for (size_t i = 0; i < model.polygons.size; i++) glDrawArrays(model.polygons.array[i].type, model.polygons.array[i].start, model.polygons.array[i].count); // drawing
}

/**
 * Renders to depth cubemap
 */
void renderToDepthMap(ShaderProgram &shaderProgram, GLuint index) {
	lightSources[index].begin();
	lightSources[index].setLightPos();
	lightSources[index].updateMatrices();
	lightSources[index].setShadowMatrices();
	glClear(GL_DEPTH_BUFFER_BIT);
		
	glEnableVertexAttribArray(shaderProgram.getValueId("a_Position"));
		
	// drawing models
	for (size_t i = 0; i < MODELS_COUNT; i++) drawModelDM(shaderProgram, models[i]);
		
	// drawing lamps
	for (GLuint i = 0; i < LAMPS_COUNT; i++) {
		if (i == index) continue;
		drawModelDM(shaderProgram, lamps[i]);
	}
		
	glDisableVertexAttribArray(shaderProgram.getValueId("a_Position"));
	
	lightSources[index].end();
}

/**
 * Color rendering
 */
void render(ShaderProgram &shaderProgram) {
    // bloom
	advren::begin();

	// view port to window size
	glViewport(0, 0, WIN_W, WIN_H);
	// updating view matrix (because camera position was changed)
	createViewMatrix();
	// sending updated matrices to vertex shader
	su::setMat4(shaderProgram.getValueId("u_View"), viewMatrix);
	su::setMat4(shaderProgram.getValueId("u_Projection"), projectionMatrix);
	// sending lamps parameters to fragment shader
	sendLampsData();
	
    glEnableVertexAttribArray(shaderProgram.getValueId("a_Position"));
    glEnableVertexAttribArray(shaderProgram.getValueId("a_Normal"));
    glEnableVertexAttribArray(shaderProgram.getValueId("a_Texture"));
	
	// drawing
	mapping.setNormalMappingEnabled(1); // with mapping
    glEnableVertexAttribArray(shaderProgram.getValueId("a_Tangent"));
    glEnableVertexAttribArray(shaderProgram.getValueId("a_Bitangent"));
	
    for (size_t i = 0; i < MODELS_COUNT; i++) drawModel(shaderProgram, models[i]);
	for (size_t i = 0; i < LAMPS_COUNT; i++) drawModel(shaderProgram, lamps[i]);
	
    glDisableVertexAttribArray(shaderProgram.getValueId("a_Position"));
    glDisableVertexAttribArray(shaderProgram.getValueId("a_Normal"));
    glDisableVertexAttribArray(shaderProgram.getValueId("a_Texture"));
    glDisableVertexAttribArray(shaderProgram.getValueId("a_Tangent"));
    glDisableVertexAttribArray(shaderProgram.getValueId("a_Bitangent"));

	advren::end();
}

void display() {
    /* --- shadow rendering --- */
    glUseProgram(shaderShadowProgram.programId);
	glUniform1f(shaderShadowProgram.getValueId("far_plane"), LAMP_FAR_PLANE);
	for (GLuint i = 0; i < LAMPS_COUNT; i++) renderToDepthMap(shaderShadowProgram, i);
	glUseProgram(0);
		
	/* --- color rendering --- */
	glUseProgram(shaderProgram.programId);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render(shaderProgram);
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
    advren::init();
    advren::SCR_W = WIN_W;
    advren::SCR_H = WIN_H;
    init_ModelBuffers_ModelMapping();
    createModels();
    initLamps();
    initShadowMaps();
    mapping.loadLocations(shaderProgram.programId);
    initDOF();

    std::thread animate_scene_th(animate_scene);

    while (!glfwWindowShouldClose(window)) {
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
        glUseProgram(shaderProgram.programId);

        if (key == GLFW_KEY_Q) {
            focalDepth++;
            glUniform1f(shaderProgram.getValueId("focalDepth"), focalDepth);
        } else if (key == GLFW_KEY_A) {
            focalDepth--;
            glUniform1f(shaderProgram.getValueId("focalDepth"), focalDepth);
        } else if (key == GLFW_KEY_W) {
            focalRange += 5;
            glUniform1f(shaderProgram.getValueId("focalRange"), focalRange);
        } else if (key == GLFW_KEY_S) {
            focalRange -= 5;
            glUniform1f(shaderProgram.getValueId("focalRange"), focalRange);
        }

        glUseProgram(0);

        std::cout << "focalDepth = " << focalDepth << "\nfocalRange = " << focalRange << "\n";
    }
}