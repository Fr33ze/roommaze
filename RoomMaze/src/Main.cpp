#include <iostream>
#include <sstream>
#include <random>

#include <physx\PxPhysicsAPI.h>
#include <inireader\INIReader.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <AL\alut.h>

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
#include "Battery.h"
#include "Resistance.h"
#include "ElevatorDoor.h"
#include "Key3D.h"
#include "ElectricBox.h"
#include "Door3D.h"
#include "Button3D.h"
#include "ButtonPanel.h"
#include "Water3D.h"
#include "Particles.h"

/* ----------- */
// PROTOTYPES
/* ----------- */

void readSettings();
GLFWwindow* initOpenGL();
void initPhysX();
void initFramebuffers();
void readObjectsFromINI(INIReader &positions, INIReader &animations, std::string &section, std::shared_ptr<Shader> shader);
void createObject(const char *path, int &type, physx::PxTransform &trans, std::shared_ptr<Shader> shader);
void initContent();
void initForFirstScreen();
void update(float deltaT);
void draw(float deltaT);
void cleanup();

void processInput(GLFWwindow *window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseMovementCallback(GLFWwindow *window, double xPos, double yPos);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void focusInteractable();

void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid *userParam);
std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char *msg);

/* ----------------- */
// GLOBAL VARIABLES
/* ----------------- */

// framebuffer stuff for bloom effect
bool bloom = true;
GLuint fboHDR, colorBuffers[2], depthBuffer, attachments[2], fboPingPong[2], colorBuffersPingPong[2];
std::shared_ptr<Shader> blurShader, bloomShader;

//settings
struct Settings {
	unsigned int width;
	unsigned int height;
	unsigned int refresh_rate;
	bool fullscreen;
	float field_of_view;
	float brightness;
} settings;

// camera
Camera *camera;
float lastXPosition;
float lastYPosition;
bool firstMouse = true;
Interactable3D *focused;

// GUI
GUI *gui;

// time between current frame and last frame
float deltaTime;

// 3d objects to render
std::vector<Object3D*> renderObjects;

// particles to render
std::vector<Particles*> renderParticles;
Particles *collectParticles;

// last generated 3d object
Object3D *lastGenerated;

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
	readSettings();

	/* ------------------------- */
	// SETTING UP OPENGL WINDOW
	/* ------------------------- */
	GLFWwindow* window = initOpenGL();

	/* ------------------------- */
	// SETTING UP OPENAL
	/* ------------------------- */
	if (!alutInit(&argc, argv))
	{
		ALenum error = alutGetError();
		fprintf(stderr, "%s\n", alutGetErrorString(error));
		exit(EXIT_FAILURE);
	}

	ALuint ambientBuffer = alutCreateBufferFromFile("assets/audio/ambient.wav");
	ALuint ambientSource;
	alGenSources(1, &ambientSource);
	alSource3f(ambientSource, AL_POSITION, 0, 0, 0);
	alSourcef(ambientSource, AL_PITCH, 1);
	alSourcef(ambientSource, AL_GAIN, 0.15);
	alSourcei(ambientSource, AL_LOOPING, AL_TRUE);
	alSourcei(ambientSource, AL_BUFFER, ambientBuffer);
	alSourcePlay(ambientSource);

	/* ----------- */
	// PHYSX INIT
	/* ----------- */
	initPhysX();

	/* ------------------------- */
	// SETTING UP CAMERA AND GUI
	/* ------------------------- */
	initForFirstScreen();

	/* --------------- */
	// MAIN GAME LOOP
	/* --------------- */

	float timeCurrentFrame = (float)glfwGetTime();
	float timeLastFrame = timeCurrentFrame;

	bool showStartScreen = true;

	while (!glfwWindowShouldClose(window)) {
		// clear the frame and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// compute the frame time delta
		timeCurrentFrame = (float)glfwGetTime();
		deltaTime = timeCurrentFrame - timeLastFrame;
		timeLastFrame = timeCurrentFrame;

		// show startscreen when content is loading
		if (showStartScreen) {
			gui->showStartScreen(true);
			gui->draw();
			glfwSwapBuffers(window);
			initContent();
			initFramebuffers();
			showStartScreen = false;
		}
		else {
			// react to user input
			glfwPollEvents();
			processInput(window);

			// update all game components
			update(deltaTime);

			// draw all game components
			draw(deltaTime);
			glfwSwapBuffers(window);

			// physx make simulation step
			pxScene->simulate(deltaTime);
			pxScene->fetchResults(true);
		}

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

void readSettings() {
	INIReader reader("assets/settings.ini");

	settings.width = reader.GetInteger("window", "width", 1280);
	settings.height = reader.GetInteger("window", "height", 720);
	settings.refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
	settings.fullscreen = reader.GetBoolean("window", "fullscreen", false);

	settings.field_of_view = reader.GetReal("camera", "fov", 50.0f);
	settings.brightness = reader.GetReal("camera", "brightness", 1.f);
}

void initForFirstScreen() {
	// mouse stuff
	lastXPosition = settings.width / 2.0f;
	lastYPosition = settings.height / 2.0f;

	// scene stuff
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// camera (includes character controller)
	camera = new Camera(glm::vec3(0.0f, 0.5f, 0.0f), settings.field_of_view, (float)settings.width / (float)settings.height);
	camera->setSpotLightParameters(settings.brightness, glm::vec3(1.0f, 1.0f, 0.95f), 0.0f, 25.0f, glm::vec3(1.0f, 0.045f, 0.0075f));

	// GUI
	gui = new GUI(settings.width, settings.height);
}

void initContent() {
	// shaders
	std::shared_ptr<Shader> phongShader = std::make_shared<Shader>("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	blurShader = std::make_shared<Shader>("assets/shaders/blur.vert", "assets/shaders/blur.frag");
	bloomShader = std::make_shared<Shader>("assets/shaders/blur.vert", "assets/shaders/bloom.frag");

	/* ------------- */
	// LOAD PARTICLES
	/* ------------- */
	Particles *electroBoxParticles = new Particles(15, 0.75f, glm::vec3(-4.325f, 1.415f, -1.687f), glm::vec3(0.55f, 0.3f, 0.0f), 0.02f, 0.1f, 2.0f, 5.0f, "assets/particles/spark.png", true);
	renderParticles.push_back(electroBoxParticles);
	collectParticles = new Particles(3, 0.45f, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f), 0.1f, 0.1f, 1.0f, 2.5f, "assets/particles/hand.png", false);
	renderParticles.push_back(collectParticles);

	/* ------------- */
	// LOAD OBJECTS (type==-1)
	/* ------------- */
	INIReader positions("assets/positions.ini");
	INIReader animations("assets/animations.ini");

	physx::PxVec3 trans = physx::PxVec3(physx::PxIdentity);
	physx::PxVec4 rot = physx::PxVec4(0.f);
	sscanf_s(positions.Get("ElevatorDoors", "0", "0 0 0 1 0 0 0").c_str(), "%f %f %f %f %f %f %f", &trans.x, &trans.y, &trans.z, &rot.w, &rot.x, &rot.y, &rot.z);
	physx::PxTransform transformation = physx::PxTransform(trans, physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
	ElevatorDoor *elevatorDoors = new ElevatorDoor(
		positions.Get("ElevatorDoors", "pathLeft", "assets/objects/elevator_left_doors.obj").c_str(),
		positions.Get("ElevatorDoors", "pathRight", "assets/objects/elevator_right_doors.obj").c_str(),
		phongShader,
		transformation
	);
	renderObjects.push_back(elevatorDoors);

	trans = physx::PxVec3(physx::PxIdentity);
	rot = physx::PxVec4(0.f);
	sscanf_s(positions.Get("Resistance", "shown", "0 0 0 1 0 0 0").c_str(), "%f %f %f %f %f %f %f", &trans.x, &trans.y, &trans.z, &rot.w, &rot.x, &rot.y, &rot.z);
	transformation = physx::PxTransform(trans, physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
	NoCollision3D *shownRes = new NoCollision3D(
		positions.Get("Resistance", "path", "assets/objects/resistance.obj").c_str(),
		phongShader,
		transformation
	);
	renderObjects.push_back(shownRes);

	trans = physx::PxVec3(physx::PxIdentity);
	rot = physx::PxVec4(0.f);
	sscanf_s(positions.Get("Resistance", "hidden", "0 0 0 1 0 0 0").c_str(), "%f %f %f %f %f %f %f", &trans.x, &trans.y, &trans.z, &rot.w, &rot.x, &rot.y, &rot.z);
	transformation = physx::PxTransform(trans, physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
	NoCollision3D *hiddenRes = new NoCollision3D(
		*shownRes,
		transformation
	);
	hiddenRes->enable(false);
	renderObjects.push_back(hiddenRes);

	trans = physx::PxVec3(physx::PxIdentity);
	rot = physx::PxVec4(0.f);
	sscanf_s(positions.Get("ElectricBox", "0", "0 0 0 1 0 0 0").c_str(), "%f %f %f %f %f %f %f", &trans.x, &trans.y, &trans.z, &rot.w, &rot.x, &rot.y, &rot.z);
	transformation = physx::PxTransform(trans, physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
	ElectricBox *electricBox = new ElectricBox(
		positions.Get("ElectricBox", "path", "assets/objects/electric_box.obj").c_str(),
		phongShader,
		transformation
	);
	electricBox->setElevatorDoor(elevatorDoors);
	electricBox->setShownRes(shownRes);
	electricBox->setHiddenRes(hiddenRes);
	electricBox->setParticle(electroBoxParticles);
	renderObjects.push_back(electricBox);

	trans = physx::PxVec3(physx::PxIdentity);
	rot = physx::PxVec4(0.f);
	sscanf_s(positions.Get("Button", "shown", "0 0 0 1 0 0 0").c_str(), "%f %f %f %f %f %f %f", &trans.x, &trans.y, &trans.z, &rot.w, &rot.x, &rot.y, &rot.z);
	transformation = physx::PxTransform(trans, physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
	NoCollision3D *shownButton = new NoCollision3D(
		positions.Get("Button", "path", "assets/objects/button.obj").c_str(),
		phongShader,
		transformation
	);
	renderObjects.push_back(shownButton);

	trans = physx::PxVec3(physx::PxIdentity);
	rot = physx::PxVec4(0.f);
	sscanf_s(positions.Get("Button", "hidden", "0 0 0 1 0 0 0").c_str(), "%f %f %f %f %f %f %f", &trans.x, &trans.y, &trans.z, &rot.w, &rot.x, &rot.y, &rot.z);
	transformation = physx::PxTransform(trans, physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
	NoCollision3D *hiddenButton = new NoCollision3D(
		*shownButton,
		transformation
	);
	hiddenButton->enable(false);
	renderObjects.push_back(hiddenButton);

	trans = physx::PxVec3(physx::PxIdentity);
	rot = physx::PxVec4(0.f);
	sscanf_s(positions.Get("ButtonPanel", "0", "0 0 0 1 0 0 0").c_str(), "%f %f %f %f %f %f %f", &trans.x, &trans.y, &trans.z, &rot.w, &rot.x, &rot.y, &rot.z);
	transformation = physx::PxTransform(trans, physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
	ButtonPanel *buttonPanel = new ButtonPanel(
		positions.Get("ButtonPanel", "path", "assets/objects/button_panel.obj").c_str(),
		phongShader,
		transformation
	);
	buttonPanel->setElevatorDoor(elevatorDoors);
	buttonPanel->setShownButton(shownButton);
	buttonPanel->setHiddenButton(hiddenButton);
	renderObjects.push_back(buttonPanel);

	/* ------------- */
	// LOAD OBJECTS (type!=-1)
	/* ------------- */
	std::set<std::string> sections = positions.Sections();
	for (std::string section : sections) {
		readObjectsFromINI(positions, animations, section, phongShader);
	}

	//starts the looping sound of electric box after loading the game, because it is FUCKING ANNOYING
	electricBox->startSound();
}

void readObjectsFromINI(INIReader &positions, INIReader &animations, std::string &section, std::shared_ptr<Shader> shader) {
	std::string path = positions.Get(section, "path", "assets/box.obj");
	int size = positions.GetInteger(section, "size", 1);
	int load = positions.GetInteger(section, "load", 1);
	int type = positions.GetInteger(section, "type", -1);
	if (type == -1)
		return;

	if (size == load) {
		for (int i = 0; i < size; i++) {
			physx::PxVec3 trans = physx::PxVec3(physx::PxIdentity);
			physx::PxVec4 rot = physx::PxVec4(0.f);
			sscanf_s(positions.Get(section, std::to_string(i), "0 0 0 1 0 0 0").c_str(), "%f %f %f %f %f %f %f", &trans.x, &trans.y, &trans.z, &rot.w, &rot.x, &rot.y, &rot.z);
			physx::PxTransform transformation = physx::PxTransform(trans, physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
			createObject(path.c_str(), type, transformation, shader);
		}
	}
	else {
		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_int_distribution<> distr(0, size - 1);
		std::vector<unsigned int> loadIds;
		for (int i = 0; i < load; i++) {
			unsigned int num = distr(eng);
			while (std::find(loadIds.begin(), loadIds.end(), num) != loadIds.end()) {
				num = distr(eng);
			}
			loadIds.push_back(num);

			physx::PxVec3 trans = physx::PxVec3(physx::PxIdentity);
			physx::PxVec4 rot = physx::PxVec4(0.f);
			sscanf_s(positions.Get(section, std::to_string(num), "0 0 0 1 0 0 0").c_str(), "%f %f %f %f %f %f %f", &trans.x, &trans.y, &trans.z, &rot.w, &rot.x, &rot.y, &rot.z);
			physx::PxTransform transformation = physx::PxTransform(trans, physx::PxQuat(rot.x, rot.y, rot.z, rot.w));
			createObject(path.c_str(), type, transformation, shader);
		}
	}

	lastGenerated = nullptr;
}

void createObject(const char *path, int &type, physx::PxTransform &trans, std::shared_ptr<Shader> shader) {
	switch (type) {

	case 0: //Battery
		if (!lastGenerated)
			lastGenerated = new Battery(
				path,
				shader,
				trans
			);
		else
			lastGenerated = new Battery(
				*((Battery*)lastGenerated),
				trans
			);
		((Battery*)lastGenerated)->setParticle(collectParticles);
		renderObjects.push_back(lastGenerated);
		break;

	case 1: //Dynamic3D
		if (!lastGenerated)
			lastGenerated = new Dynamic3D(
				path,
				shader,
				trans
			);
		else
			lastGenerated = new Dynamic3D(
				*((Dynamic3D*)lastGenerated),
				trans
			);
		renderObjects.push_back(lastGenerated);
		break;

	case 2: //Button3D
		if (!lastGenerated)
			lastGenerated = new Button3D(
				path,
				shader,
				trans
			);
		else
			lastGenerated = new Button3D(
				*((Button3D*)lastGenerated),
				trans
			);
		((Button3D*)lastGenerated)->setParticle(collectParticles);
		renderObjects.push_back(lastGenerated);
		break;

	case 4: //Door
		if (!lastGenerated)
			lastGenerated = new Door3D(
				path,
				shader,
				trans
			);
		else
			lastGenerated = new Door3D(
				*((Door3D*)lastGenerated),
				trans
			);
		renderObjects.push_back(lastGenerated);
		break;
	case 6: //Water3D
		if (!lastGenerated)
			lastGenerated = new Water3D(
				path,
				shader,
				trans
			);
		else
			lastGenerated = new Water3D(
				*((Water3D*)lastGenerated),
				trans
			);
		renderObjects.push_back(lastGenerated);
		break;

	case 7: //Key
		if (!lastGenerated)
			lastGenerated = new Key3D(
				path,
				shader,
				trans
			);
		else
			lastGenerated = new Key3D(
				*((Key3D*)lastGenerated),
				trans
			);
		((Key3D*)lastGenerated)->setParticle(collectParticles);
		renderObjects.push_back(lastGenerated);
		break;

	case 8: //Resistance
		if (!lastGenerated)
			lastGenerated = new Resistance(
				path,
				shader,
				trans
			);
		else
			lastGenerated = new Resistance(
				*((Resistance*)lastGenerated),
				trans
			);
		((Resistance*)lastGenerated)->setParticle(collectParticles);
		renderObjects.push_back(lastGenerated);
		break;

	case 9: //Static3D
		if (!lastGenerated)
			lastGenerated = new Static3D(
				path,
				shader,
				trans
			);
		else
			lastGenerated = new Static3D(
				*((Static3D*)lastGenerated),
				trans
			);
		renderObjects.push_back(lastGenerated);
		break;
	}
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

void initFramebuffers() {
	glGenFramebuffers(1, &fboHDR);
	glBindFramebuffer(GL_FRAMEBUFFER, fboHDR);

	// create color buffers (one for normal rendering, other for brightness treshold values)
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, settings.width, settings.height, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// clamp to the edge otherwise the blur filter will sample repeated texture values
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, settings.width, settings.height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;
	glDrawBuffers(2, attachments);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glfwTerminate();
		std::cout << "Framebuffer not complete." << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring
	glGenFramebuffers(2, fboPingPong);
	glGenTextures(2, colorBuffersPingPong);
	for (unsigned int i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, fboPingPong[i]);
		glBindTexture(GL_TEXTURE_2D, colorBuffersPingPong[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, settings.width, settings.height, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// clamp to the edge otherwise the blur filter will sample repeated texture values
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffersPingPong[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			glfwTerminate();
			std::cout << "Framebuffer not complete." << std::endl;
			std::cout << "Press ENTER to close this window." << std::endl;
			getchar();
			exit(-1);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void update(float deltaT) {
	camera->move(deltaT);
	gui->updateTime(deltaT);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
	if (quadVAO == 0) {
		float quadVertices[] = {
			// positions        // texture coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);

		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void draw(float deltaT) {
	/* ---------------------------------------------- */
	//  RENDER SCENE INTO FLOATING POINT FRAMEBUFFER
	/* ---------------------------------------------- */
	glBindFramebuffer(GL_FRAMEBUFFER, fboHDR);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// draw all game components
	for (unsigned int i = 0; i < renderObjects.size(); i++) {
		renderObjects.at(i)->draw(deltaT);
	}
	for (unsigned int i = 0; i < renderParticles.size(); i++) {
		renderParticles.at(i)->draw(deltaT);
	}
	gui->draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* --------------------------------------------------- */
	//  BLUR BRIGHT FRAGMENTS WITH TWO-PASS GAUSSIAN BLUR
	/* --------------------------------------------------- */
	bool horizontal = true, firstIteration = true;
	unsigned int amount = 10;
	blurShader->use();
	for (unsigned int i = 0; i < amount; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, fboPingPong[horizontal]);
		blurShader->setUniform("horizontal", horizontal);
		glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : colorBuffersPingPong[!horizontal]); // bind texture of other framebuffer (or scene if first iteration)
		blurShader->setUniform("image", 0);
		renderQuad();

		horizontal = !horizontal;
		if (firstIteration)
			firstIteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* --------------------------------------------------------------------------------------------------------------------- */
	//  RENDER FLOATING POINT COLOR BUFFER TO 2D QUAD AND TONEMAP HDR COLORS TO DEFAULT FRAMEBUFFER'S (CLAMPED) COLOR RANGE
	/* --------------------------------------------------------------------------------------------------------------------- */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	bloomShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	bloomShader->setUniform("scene", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, colorBuffersPingPong[!horizontal]);
	bloomShader->setUniform("bloomBlur", 1);
	bloomShader->setUniform("bloom", bloom);
	renderQuad();
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
	// SPACE = insert new battery into the camera
	// B = toggle bloom
	// F10 = get all items

	if (action != GLFW_RELEASE)
		return;

	switch (key) {
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, true);
		break;
	case GLFW_KEY_SPACE:
		gui->removeBattery();
		break;
	case GLFW_KEY_B:
		bloom = !bloom;
		break;
	case GLFW_KEY_F10:
		gui->setInfoText("I'm a noob and need cheats!");
		gui->addBattery();
		gui->addButton();
		gui->addKey();
		gui->addResistance();
	}
}

void processInput(GLFWwindow *window) {
	// W = move character forward
	// S = move character backward
	// A = move character left
	// D = move character right

	bool processInteractables = false;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera->processKeyEvent(W, glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS, deltaTime);
		processInteractables = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera->processKeyEvent(S, glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS, deltaTime);
		processInteractables = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera->processKeyEvent(A, glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS, deltaTime);
		processInteractables = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera->processKeyEvent(D, glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS, deltaTime);
		processInteractables = true;
	}
	if (processInteractables) {
		focusInteractable();
	}
}

void focusInteractable() {
	physx::PxRaycastBuffer hit;
	if (camera->raycast(hit)) {
		if (hit.hasBlock) {
			focused = (Interactable3D*)hit.block.actor->userData;
			gui->setInfoText(focused->guitext(gui));
		}
		else {
			focused = nullptr;
			gui->setInfoText("");
		}
	}
	else {
		focused = nullptr;
		gui->setInfoText("");
	}
}

void mouseMovementCallback(GLFWwindow *window, double xPos, double yPos) {
	if (firstMouse) {
		lastXPosition = (float)xPos;
		lastYPosition = (float)yPos;
		firstMouse = false;
	}

	float xOffset = (float)xPos - lastXPosition;
	float yOffset = lastYPosition - (float)yPos; // reversed since y-coordinates go from bottom to top

	lastXPosition = (float)xPos;
	lastYPosition = (float)yPos;

	camera->processMouseMovement(xOffset, yOffset);

	focusInteractable();
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (focused) {
			focused->use(gui);
			gui->setInfoText(focused->guitext(gui));
		}
	}
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