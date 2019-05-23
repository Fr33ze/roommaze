#include "GUI.h"

GUI::GUI(int windowWidth, int windowHeight, int startingBatteries, Camera *camera)
	: batteryCounter(startingBatteries), batteryTime(120.0f), camera(camera) {

	componentShader = std::make_shared<Shader>("assets/shaders/gui.vert", "assets/shaders/guiComponent.frag");
	textShader = std::make_shared<Shader>("assets/shaders/gui.vert", "assets/shaders/guiText.frag");
	
	battery = GUIComponent("assets/gui/battery.png", glm::vec2(windowWidth * 0.002f, windowHeight * 0.98f), 0.05f, windowWidth, windowHeight);

	std::string fontPath = "assets/gui/Roboto-Regular.ttf";

	batteryStatus = GUIText(fontPath, std::to_string(startingBatteries) + "/10", glm::vec2(windowWidth * 0.055f, windowHeight * 0.93f), windowWidth / 1920.0f * 0.5f, 0.75f, windowWidth, windowHeight);
	batteryCountdown = GUIText(fontPath, std::to_string((int)(batteryTime / 1.2f)) + " %", glm::vec2(windowWidth * 0.055f, windowHeight * 0.895f), windowWidth / 1920.0f * 0.3f, 0.75f, windowWidth, windowHeight);
	infoText = GUIText(fontPath, "", glm::vec2(windowWidth * 0.015f, windowHeight * 0.03f), windowWidth / 1920.0f * 0.4f, 1.0f, windowWidth, windowHeight);
}

GUI::GUI() {

}

GUI::~GUI() {

}

void GUI::destroy() {
	
}

void GUI::updateTime(float deltaT) {
	if (batteryTime - deltaT > 0.0f) {
		batteryTime -= deltaT;
		batteryCountdown.updateText(std::to_string((int)(batteryTime / 1.2f)) + " %");

		if (batteryTime <= 2.0f) {
			if ((int)(batteryTime * 100) % 2 == 0) { // TODO: random flickering
				camera->setSpotLightParameters(glm::vec3(0.0f), 0.0f, 25.0f, glm::vec3(0.4f));
			}
			else {
				camera->setSpotLightParameters(glm::vec3(0.8f), 0.0f, 25.0f, glm::vec3(0.4f));
			}
		}
	}
	else {
		camera->setSpotLightParameters(glm::vec3(0.0f), 0.0f, 25.0f, glm::vec3(0.4f));
	}
}

void GUI::draw() {
	battery.draw(componentShader);
	batteryStatus.draw(textShader);

	if (!(batteryTime <= 10.0f && (int)(batteryTime * 2) % 2 == 0) || batteryTime <= 0.5f) {
		batteryCountdown.draw(textShader);
	}

	infoText.draw(textShader);
}

void GUI::addBattery() {
	batteryCounter++;
	batteryStatus.updateText(std::to_string(batteryCounter) + "/10");
}

void GUI::deleteBattery() {
	if (batteryCounter > 0) {
		batteryCounter--;
		batteryStatus.updateText(std::to_string(batteryCounter) + "/10");
		batteryTime = 120.0f;
		camera->setSpotLightParameters(glm::vec3(0.8f), 0.0f, 25.0f, glm::vec3(0.4f));
	}
}

void GUI::setInfoText(std::string text) {
	infoText.updateText(text);
}
