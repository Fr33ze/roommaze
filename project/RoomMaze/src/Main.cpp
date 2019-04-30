#include <iostream>
#include <sstream>

#include <physx\PxPhysicsAPI.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "Object3D.h"
#include "OBJReader.h"
#include "INIReader.h"

/* ----------- */
// PROTOTYPES
/* ----------- */

void init();
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

// time between current frame and last frame
float deltaTime;

// 3d objects to render
std::vector<Object3D*> renderObjects;

// physX
static physx::PxDefaultErrorCallback gErrorCallback;
static physx::PxDefaultAllocator gAllocatorCallback;
physx::PxFoundation *mFoundation;
physx::PxPhysics *mPhysics;
physx::PxPvd *mPvd;
physx::PxCooking *mCooking;


/* ----- */
// MAIN
/* ----- */

int main(int argc, char **argv) {
	// read settings.ini file
	settings = INIReader::ReadSettings();


	/* ------------------------- */
	// SETTING UP OPENGL WINDOW
	/* ------------------------- */

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

	/* --------------- */
    // MAIN GAME LOOP
	/* --------------- */

	float timeCurrentFrame = (float) glfwGetTime();
	float timeLastFrame = timeCurrentFrame;

	// init game content
	init();

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

		// check for errors
		if (glGetError() != GL_NO_ERROR)
			std::cout << "GL ERROR DETECTED!!!" << std::endl;
	}

	// clean up
	cleanup();
	glfwTerminate();

	return EXIT_SUCCESS;
}

void init() {
	// mouse stuff
	lastXPosition = settings.width / 2.0f;
	lastYPosition = settings.height / 2.0f;

	// scene stuff
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// shader
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("assets/shaders/phong.vert", "assets/shaders/phong.frag");

	// camera
	camera = Camera(glm::vec3(0.0f, 1.20f, 7.0f), settings.field_of_view, (float) settings.width / (float) settings.height);
	camera.setSpotLightParameters(glm::vec3(1.0f), 0.0f, 25.0f, glm::vec3(0.2f, 0.4f, 0.2f));


	/* --------------- */
	// PHYSX INIT
	/* --------------- */

	gErrorCallback = physx::PxDefaultErrorCallback();
	gAllocatorCallback = physx::PxDefaultAllocator();

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocatorCallback, gErrorCallback);
	if (!mFoundation) {
		std::cout << "PxCreateFoundation failed!" << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	/* PHYSX VISUAL DEBUGGER (physx extension needed) */
	/*bool recordMemoryAllocations = true;
	mPvd = PxCreatePvd(*mFoundation);
	physx::PxPvdTransport *transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);*/

	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale());
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

	/* PHYSX INITIALIZE EXTENSIONS (only needed for PVD???)*/
	/*if (!PxInitExtensions(*mPhysics, mPvd)) {
		std::cout << "PxInitExtensions failed!" << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}*/


	/* ------------- */
	// LOAD OBJECTS
	/* ------------- */

	renderObjects.push_back(new Object3D("assets/objects/staircase/staircase.obj", shader));

	renderObjects.push_back(new Object3D("assets/objects/shelf/shelf.obj", shader, glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 2.4f))));

	renderObjects.push_back(new Object3D("assets/objects/cube/cube.obj", shader, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 4.5f))));
	
}

void update(float deltaT) {

}

void draw() {
	for (unsigned int i = 0; i < renderObjects.size(); i++) {
		renderObjects.at(i)->draw(camera);
	}
}

void cleanup() {
	for (unsigned int i = 0; i < renderObjects.size(); i++) {
		renderObjects.at(i)->destroy();
	}
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
		std::cout << "INSERT NEW BATTERY" << std::endl;
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
