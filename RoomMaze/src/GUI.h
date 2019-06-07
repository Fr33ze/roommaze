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

	unsigned int lastBatteryCount;
	float batteryTime;
	GUIComponent battery;
	GUIText batteryStatus, batteryCountdown, infoText;

public:
	// inventory of player
	struct Inventory {
		unsigned int batteries = 5;
		unsigned int keys = 0;
		bool button = false;
		bool resistance = false;
	} *inv;

	/**
	 * CONSTRUCTOR
	 * Creates the GUI of the game.
	 *
	 * @param windowWidth: the width of the window
	 * @param windowHeight: the height of the window
	 * @param inv: inventory of the player
	 */
	GUI(int windowWidth, int windowHeight, Inventory *inv);
	
	GUI();

	~GUI();

	void destroy();

	void updateTime(float deltaT);

	void draw();

	void setInfoText(std::string text);

};
