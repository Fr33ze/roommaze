#include <iostream>
#include <sstream>

#include <physx\PxPhysicsAPI.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#ifndef _DEBUG
#include <Windows.h>
#endif

#include "Shader.h"
#include "Camera.h"
#include "GUI.h"
#include "Material.h"
#include "Object3D.h"
#include "Static3D.h"
#include "Dynamic3D.h"
#include "OBJReader.h"
#include "INIReader.h"

/* ----------- */
// PROTOTYPES
/* ----------- */

GLFWwindow* initOpenGL();
void initPhysX();
void initContent();
void update(float deltaT);
void draw();
void cleanup();

void processInput(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseMovementCallback(GLFWwindow *window, double xPos, double yPos);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid *userParam);
std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char *msg);

/* ----------------- */
// GLOBAL VARIABLES
/* ----------------- */

//settings
INIReader::Settings settings;

// camera
Camera camera;
float lastXPosition;
float lastYPosition;
bool firstMouse = true;

// GUI
GUI gui;

// time between current frame and last frame
float deltaTime;

// 3d objects to render
std::vector<Object3D*> renderObjects;

// physX
physx::PxDefaultErrorCallback gErrorCallback;
physx::PxDefaultAllocator gAllocator;
physx::PxDefaultCpuDispatcher *gDispatcher;
physx::PxFoundation *mFoundation;
physx::PxPhysics *mPhysics;
physx::PxCooking *mCooking;
physx::PxScene *pxScene;

#if _DEBUG
physx::PxPvd *mPvd;
physx::PxPvdTransport *mTransport;
#endif

/* ----- */
// MAIN
/* ----- */

int main(int argc, char **argv) {

#ifndef _DEBUG
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif

	/* ------------------------- */
	// READ SETTINGS.INI FILE
	/* ------------------------- */
	settings = INIReader::readSettings();

	/* ------------------------- */
	// SETTING UP OPENGL WINDOW
	/* ------------------------- */
	GLFWwindow* window = initOpenGL();

	/* ----------- */
	// PHYSX INIT
	/* ----------- */
	initPhysX();

	/* --------------- */
	// GAME CONTENT INIT
	/* --------------- */
	initContent();

	/* --------------- */
	// MAIN GAME LOOP
	/* --------------- */

	float timeCurrentFrame = (float)glfwGetTime();
	float timeLastFrame = timeCurrentFrame;

	while (!glfwWindowShouldClose(window)) {
		// clear the frame and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// compute the frame time delta
		timeCurrentFrame = (float) glfwGetTime();
		deltaTime = timeCurrentFrame - timeLastFrame;
		timeLastFrame = timeCurrentFrame;

		// react to user input
		glfwPollEvents();
		processInput(window);

		// update all game components
		update(deltaTime);

		// draw all game components
		draw();
		glfwSwapBuffers(window);

		// physx make simulation step
		pxScene->simulate(1.0f / 60.0f);
		pxScene->fetchResults(true);

		// check for errors
		if (glGetError() != GL_NO_ERROR)
			std::cout << "GL ERROR DETECTED!!!" << std::endl;
	}

	/* --------------- */
	// CLEAN UP
	/* --------------- */
	cleanup();
	glfwTerminate();

	return EXIT_SUCCESS;
}

void initContent() {
	// mouse stuff
	lastXPosition = settings.width / 2.0f;
	lastYPosition = settings.height / 2.0f;

	// scene stuff
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// shader
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("assets/shaders/phong.vert", "assets/shaders/phong.frag");

	// camera (includes character controller)
	camera = Camera(glm::vec3(0.0f, 2.0f, 0.0f), settings.field_of_view, (float)settings.width / (float)settings.height);
	camera.setSpotLightParameters(1.0f, glm::vec3(1.0f, 1.0f, 0.95f), 0.0f, 25.0f, glm::vec3(1.0f, 0.045f, 0.0075f));

	// GUI
	gui = GUI(settings.width, settings.height, 5, &camera);

	/* ------------- */
	// LOAD OBJECTS
	/* ------------- */

	//Construct a static (non moveable) cube with the given initial transformation
	//Static3D *staticCube = new Static3D("assets/objects/cube/cube.obj", shader, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.125f, -3.0f)));
	//renderObjects.push_back(staticCube);

	//Construct a dynamic (moveable and affected by gravity) cube that spawns on top of the static cube
	Dynamic3D *dynamicCube = new Dynamic3D("assets/objects/cube/cube.obj", shader, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -3.0f)));
	renderObjects.push_back(dynamicCube);
	Dynamic3D *dynamicCube2 = new Dynamic3D("assets/objects/cube/cube.obj", shader, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.3f, -3.0f)));
	renderObjects.push_back(dynamicCube2);
	Dynamic3D *dynamicCube3 = new Dynamic3D("assets/objects/cube/cube.obj", shader, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.6f, -3.0f)));
	renderObjects.push_back(dynamicCube3);
	Dynamic3D *dynamicCube4 = new Dynamic3D("assets/objects/cube/cube.obj", shader, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.9f, -3.0f)));
	renderObjects.push_back(dynamicCube4);

	Static3D *maze = new Static3D("assets/objects/maze/maze.obj", shader);
	renderObjects.push_back(maze);
	Static3D *mazeWater = new Static3D("assets/objects/maze/mazeWater.obj", shader);
	renderObjects.push_back(mazeWater);

	Static3D *battery = new Static3D("assets/objects/battery/battery.obj", shader, glm::translate(glm::mat4(1.0f), glm::vec3(-0.2f, 1.3f, 3.3f)));
	renderObjects.push_back(battery);
}

GLFWwindow* initOpenGL() {
	// init glfw
	if (!glfwInit()) {
		std::cout << "Failed to init glfw." << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	// set window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if _DEBUG
	// Create a debug OpenGL context.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_REFRESH_RATE, settings.refresh_rate);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	// open window
	GLFWwindow* window = glfwCreateWindow(settings.width, settings.height, "RoomMaze", settings.fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		std::cout << "Failed to open window." << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}
	glViewport(0, 0, settings.width, settings.width);

	glfwMakeContextCurrent(window);

	// init glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize glew." << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	/* ---------------------------- */
	// REGISTER CALLBACK FUNCTIONS
	/* ---------------------------- */

#if _DEBUG
	// Query the OpenGL function to register your callback function.
	PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)glfwGetProcAddress("glDebugMessageCallback");
	PFNGLDEBUGMESSAGECALLBACKARBPROC glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC)glfwGetProcAddress("glDebugMessageCallbackARB");

	// Register your callback function.
	if (glDebugMessageCallback != NULL) {
		glDebugMessageCallback(DebugCallback, NULL);
	}
	else if (glDebugMessageCallbackARB != NULL) {
		glDebugMessageCallbackARB(DebugCallback, NULL);
	}

	// Enable synchronous callback. This ensures that your callback function is called
	// right after an error has occurred. This capability is not defined in the AMD
	// version.
	if ((glDebugMessageCallback != NULL) || (glDebugMessageCallbackARB != NULL)) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}
#endif

	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseMovementCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return window;
}

void initPhysX() {
	gErrorCallback = physx::PxDefaultErrorCallback();
	gAllocator = physx::PxDefaultAllocator();

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	if (!mFoundation) {
		std::cout << "PxCreateFoundation failed!" << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

#if _DEBUG
	/* PHYSX VISUAL DEBUGGER */
	mPvd = PxCreatePvd(*mFoundation);
	mTransport = physx::PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);
	mPvd->connect(*mTransport, physx::PxPvdInstrumentationFlag::eALL);

	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), true, mPvd);
#else
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), true);
#endif
	if (!mPhysics) {
		std::cout << "PxCreatePhysics failed!" << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, physx::PxCookingParams(mPhysics->getTolerancesScale()));
	if (!mCooking) {
		std::cout << "PxCreateCooking failed!" << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

#if _DEBUG
	/* PHYSX INITIALIZE EXTENSIONS (only needed for PVD???)*/
	if (!PxInitExtensions(*mPhysics, mPvd)) {
		std::cout << "PxInitExtensions failed!" << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}
#endif

	physx::PxSceneDesc sceneDesc = physx::PxSceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = physx::PxDefaultCpuDispatcherCreate(4);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	pxScene = mPhysics->createScene(sceneDesc);
}

void update(float deltaT) {
	camera.move(deltaT);
	gui.updateTime(deltaT);
}

void draw() {
	for (unsigned int i = 0; i < renderObjects.size(); i++) {
		renderObjects.at(i)->draw(camera);
	}

	gui.draw();
}

void cleanup() {
	for (unsigned int i = 0; i < renderObjects.size(); i++) {
		renderObjects.at(i)->destroy();
	}

	mPhysics->release();

#if _DEBUG
	mPvd->release();
	mTransport->release();
#endif
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	// ESC = exit
	// F = toggle nightvision on / off
	// SPACE = insert new battery into the camera

	if (action != GLFW_RELEASE)
		return;

	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_F:
		std::cout << "TOGGLE NIGHTVISION" << std::endl;
		break;
	case GLFW_KEY_SPACE:
		gui.deleteBattery();
		break;
	}
}

void processInput(GLFWwindow *window) {
	// W = move character forward
	// S = move character backward
	// A = move character left
	// D = move character right
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyEvent(W, glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyEvent(S, glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyEvent(A, glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyEvent(D, glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS, deltaTime);
}

void mouseMovementCallback(GLFWwindow *window, double xPos, double yPos) {
	if (firstMouse) {
		lastXPosition = (float) xPos;
		lastYPosition = (float) yPos;
		firstMouse = false;
	}

	float xOffset = (float) xPos - lastXPosition;
	float yOffset = lastYPosition - (float) yPos; // reversed since y-coordinates go from bottom to top
	
	lastXPosition = (float) xPos;
	lastYPosition = (float) yPos;

	camera.processMouseMovement(xOffset, yOffset);
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {

}

void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid *userParam) {
	if (id == 131185 || id == 131218) return; // ignore performance warnings from NVIDIA
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char *msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
	switch (source) {
	case GL_DEBUG_CATEGORY_API_ERROR_AMD:
	case GL_DEBUG_SOURCE_API: {
		sourceString = "API";
		break;
	}
	case GL_DEBUG_CATEGORY_APPLICATION_AMD:
	case GL_DEBUG_SOURCE_APPLICATION: {
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		sourceString = "Third Party";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_SOURCE_OTHER: {
		sourceString = "Other";
		break;
	}
	default: {
		sourceString = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		typeString = "Error";
		break;
	}
	case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_TYPE_OTHER: {
		typeString = "Other";
		break;
	}
	default: {
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severityString = "Low";
		break;
	}
	default: {
		severityString = "Unknown";
		break;
	}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}
