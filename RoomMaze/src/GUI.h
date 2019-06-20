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

	float batteryTime;
	GUIComponent battery, keyComp, resistanceComp, buttonComp, startScreen, gameOverScreen, endScreen;
	bool startScreenIsEnabled, gameOverScreenIsEnabled, endScreenIsEnabled;
	GUIText batteryStatus, batteryCountdown, infoText;

	// width (X-resolution) of the window
	int windowWidth;

	// inventory of player
	unsigned int batteries = 5;
	bool key = false;
	bool button = false;
	bool resistance = false;

	float overtime = 0.f;
	ALuint audioSource;
	ALuint ghostBuffer;
	bool gameoveraudioplayed = false;
public:

	/**
	 * CONSTRUCTOR
	 * Creates the GUI of the game.
	 *
	 * @param windowWidth: the width of the window
	 * @param windowHeight: the height of the window
	 */
	GUI(int windowWidth, int windowHeight);
	
	GUI();

	~GUI();

	void destroy();

	void updateTime(float deltaT);

	void draw();

	void setInfoText(std::string text);

	void showStartScreen(bool b);
	void showGameOverScreen(bool b);
	void showEndScreen(bool b);

	void addBattery(bool isCheater);
	void removeBattery();
	unsigned int getBatteries();
	void addKey();
	void addButton();
	void addResistance();
	void removeResistance();
	void removeButton();
	bool hasKey();
	bool hasResistance();
	bool hasButton();
};
