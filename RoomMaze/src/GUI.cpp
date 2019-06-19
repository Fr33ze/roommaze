#include "GUI.h"

GUI::GUI(int windowWidth, int windowHeight)
	: batteryTime(120.0f), windowWidth(windowWidth) {

	componentShader = std::make_shared<Shader>("assets/shaders/gui.vert", "assets/shaders/guiComponent.frag");
	textShader = std::make_shared<Shader>("assets/shaders/gui.vert", "assets/shaders/guiText.frag");
	
	battery = GUIComponent("assets/gui/battery.png", glm::vec2(windowWidth * 0.002f, windowHeight * 0.98f), 0.05f, windowWidth, windowHeight, false);
	startScreen = GUIComponent("assets/gui/start_screen.jpg", glm::vec2(0.0f, windowHeight), 1.0f, windowWidth, windowHeight, true);
	gameOverScreen = GUIComponent("assets/gui/game_over_screen.jpg", glm::vec2(0.0f, windowHeight), 1.0f, windowWidth, windowHeight, true);
	endScreen = GUIComponent("assets/gui/end_screen.jpg", glm::vec2(0.0f, windowHeight), 1.0f, windowWidth, windowHeight, true);

	std::string fontPath = "assets/gui/Roboto-Regular.ttf";

	batteryStatus = GUIText(fontPath, std::to_string(batteries) + "/10", glm::vec2(windowWidth * 0.055f, windowHeight * 0.93f), windowWidth / 1920.0f * 0.5f, 0.75f, windowWidth, windowHeight);
	batteryCountdown = GUIText(fontPath, std::to_string((int)(batteryTime / 1.2f)) + " %", glm::vec2(windowWidth * 0.055f, windowHeight * 0.895f), windowWidth / 1920.0f * 0.3f, 0.75f, windowWidth, windowHeight);
	infoText = GUIText(fontPath, "", glm::vec2(windowWidth * 0.015f, windowHeight * 0.8f), windowWidth / 1920.0f * 0.5f, 0.75f, windowWidth, windowHeight);
}

GUI::GUI() {

}

GUI::~GUI() {

}

void GUI::destroy() {
	
}

void GUI::updateTime(float deltaT) {
	extern Camera *camera;
	if (batteryTime - deltaT > 0.0f) {
		batteryTime -= deltaT;
		batteryCountdown.updateText(std::to_string((int)(batteryTime / 1.2f)) + " %");

		if (batteryTime <= 2.0f) {
			if ((int)(batteryTime * 100) % 2 == 0) { // TODO: random flickering
				camera->turnSpotlightOff();
			}
			else {
				camera->turnSpotlightOn();
			}
		}
	}
	else {
		camera->turnSpotlightOff();
	}
}

void GUI::draw() {
	if (startScreenIsEnabled)
		startScreen.draw(componentShader);
	else if (gameOverScreenIsEnabled)
		gameOverScreen.draw(componentShader);
	else if (endScreenIsEnabled)
		endScreen.draw(componentShader);
	else {
		battery.draw(componentShader);
		batteryStatus.draw(textShader);

		if (!(batteryTime <= 10.0f && (int)(batteryTime * 2) % 2 == 0) || batteryTime <= 0.5f) {
			batteryCountdown.draw(textShader);
		}

		infoText.draw(textShader);
	}
}

void GUI::setInfoText(std::string text) {
	infoText.updateTextCenter(text);
}

void GUI::showStartScreen(bool b) {
	startScreenIsEnabled = b;
}

void GUI::showGameOverScreen(bool b) {
	gameOverScreenIsEnabled = b;
}

void GUI::showEndScreen(bool b) {
	endScreenIsEnabled = b;
}

void GUI::addBattery()
{
	batteries++;
	batteryStatus.updateText(std::to_string(batteries) + "/10");
}

void GUI::removeBattery()
{
	batteries--;
	if (batteries > 0) {
		extern Camera *camera;
		batteryStatus.updateText(std::to_string(batteries) + "/10");
		batteryTime = 120.0f;
		camera->turnSpotlightOn();
	}
}

unsigned int GUI::getBatteries()
{
	return batteries;
}

void GUI::addKey()
{
	key = true;
}

void GUI::addButton()
{
	button = true;
}

void GUI::addResistance()
{
	resistance = true;
}

void GUI::removeResistance()
{
	resistance = false;
}

void GUI::removeButton()
{
	button = false;
}

bool GUI::hasKey()
{
	return key;
}

bool GUI::hasResistance()
{
	return resistance;
}

bool GUI::hasButton()
{
	return button;
}
