#pragma once

#include <iostream>
#include <sstream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <map>

#include "Shader.h"
#include "Camera.h"
#include "GUIComponent.h"
#include "GUIText.h"

class GUI {

protected:

	// shader used for rendering gui components
	std::shared_ptr<Shader> componentShader;
	// shader used for rendering text
	std::shared_ptr<Shader> textShader;

	float batteryTime, collapseTime;

	int batteryCounter;
	GUIComponent battery;
	GUIText batteryStatus, batteryCountdown, collapseCountdown, infoText;

	// camera
	Camera *camera;

public:
	
	/**
	 * CONSTRUCTOR
	 * Creates the GUI of the game.
	 *
	 * @param windowWidth: the width of the window
	 * @param windowHeight: the height of the window
	 * @param staringBatteries: amount of batteries to start with
	 * @param camera: camera of the game
	 */
	GUI(int windowWidth, int windowHeight, int startingBatteries, Camera *camera);
	
	GUI();

	~GUI();

	void destroy();

	void updateTime(float deltaT);

	void draw();

	void addBattery();

	void deleteBattery();

	void setInfoText(std::string text);

};
