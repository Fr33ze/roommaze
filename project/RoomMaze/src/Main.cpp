#include <iostream>
#include <sstream>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Material.h"
#include "LightCollection.h"
#include "Geometry.h"

/* -------------------------------------------------- */
// PROTOTYPES
/* -------------------------------------------------- */

static void init();
static void update(float deltaT);
static void draw();
static void cleanup();

static void processInput(GLFWwindow *window);
static void mouseMovementCallback(GLFWwindow *window, double xPos, double yPos);
static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid *userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char *msg);

/* -------------------------------------------------- */
// GLOBAL VARIABLES
/* -------------------------------------------------- */

// settings
static const unsigned int WINDOW_WIDTH = 1000;
static const unsigned int WINDOW_HEIGHT = 500;
static const bool FULLSCREEN = false;

// camera
Camera camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastXPosition = WINDOW_WIDTH / 2.0f;
float lastYPosition = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

// time between current frame and last frame
float deltaTime;

// geometry objects
std::vector<Geometry> geometries;

/* -------------------------------------------------- */
// MAIN
/* -------------------------------------------------- */

int main(int argc, char **argv) {

	/* -------------------------------------------------- */
	// SETTING UP OPENGL WINDOW
	/* -------------------------------------------------- */

	// init glfw
	if (!glfwInit()) {
		std::cout << "Failed to init glfw." << std::endl;
		system("PAUSE");
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
	glfwWindowHint(GLFW_REFRESH_RATE, 60);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// open window
	GLFWwindow* window = glfwCreateWindow(1000, 500, "RoomMaze", FULLSCREEN ? glfwGetPrimaryMonitor() : nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		std::cout << "Failed to open window." << std::endl;
		system("PAUSE");
		exit(-1);
	}

	glfwMakeContextCurrent(window);

	// init glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize glew." << std::endl;
		system("PAUSE");
		exit(-1);
	}

	/* -------------------------------------------------- */
	// REGISTER CALLBACK FUNCTIONS
	/* -------------------------------------------------- */

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

	glfwSetCursorPosCallback(window, mouseMovementCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/* -------------------------------------------------- */
    // MAIN GAME LOOP
	/* -------------------------------------------------- */

	float timeCurrentFrame = (float) glfwGetTime();
	float timeLastFrame = timeCurrentFrame;

	// init game content
	init();






	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// shader
	std::shared_ptr<Shader> shader = std::make_shared<Shader>("assets/shader/phong.vert", "assets/shader/phong.frag");

	// materials
	std::string path = "";
	std::shared_ptr<Material> material = std::make_shared<Material>(shader, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 100.0f, 1.0f, path, path, path, path, path);

	// light collection
	LightCollection lightCollection = LightCollection(shader);
	lightCollection.addDirectionalLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.0f));

	// objects
	GeometryData geometryData = Geometry::createCube(1.5f, 1.5f, 1.5f);
	Geometry geometry = Geometry(geometryData, shader, material, std::make_shared<LightCollection>(lightCollection), glm::mat4(1.0f));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	





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
		geometry.setUniformsAndDraw(camera);
		glfwSwapBuffers(window);

		// check for errors
		if (glGetError() != GL_NO_ERROR)
			std::cout << "GL ERROR DETECTED!!!" << std::endl;
	}

	// clean up!
	cleanup();
	glfwTerminate();

	return EXIT_SUCCESS;
}

static void init() {
	
}

static void update(float deltaT) {

}

static void draw() {

}

static void cleanup() {
	
}

static void processInput(GLFWwindow *window) {
	// ESC = exit
	// W = move character forward
	// S = move character backward
	// A = move character left
	// D = move character right
	// F = toggle nightvision on / off
	// SPACE = insert new battery into the camera
	
	float cameraSpeed = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyEvent(W, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyEvent(S, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyEvent(A, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyEvent(D, deltaTime);
}

static void mouseMovementCallback(GLFWwindow *window, double xPos, double yPos) {
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

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {

}

static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid *userParam) {
	if (id == 131185 || id == 131218) return; // ignore performance warnings from NVIDIA
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char *msg) {
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
