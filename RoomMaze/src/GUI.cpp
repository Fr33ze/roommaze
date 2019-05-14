#include "GUI.h"

GUI::GUI(int windowWidth, int windowHeight, int startingBatteries, Camera *camera)
	: batteryTime(60.0f), collapseTime(120.0f), batteryCounter(startingBatteries), camera(camera) {

	componentShader = std::make_shared<Shader>("assets/shaders/gui.vert", "assets/shaders/guiComponent.frag");
	textShader = std::make_shared<Shader>("assets/shaders/gui.vert", "assets/shaders/guiText.frag");
	
	battery = GUIComponent("assets/gui/battery.png", glm::vec2(windowWidth * 0.002f, windowHeight * 0.98f), 0.05f, windowWidth, windowHeight);

	std::string fontPath = "assets/gui/Roboto-Regular.ttf";

	batteryStatus = GUIText(fontPath, std::to_string(startingBatteries) + "/10", glm::vec2(windowWidth * 0.055f, windowHeight * 0.93f), windowWidth / 1920.0f * 0.5f, windowWidth, windowHeight);
	batteryCountdown = GUIText(fontPath, std::to_string((int)batteryTime) + " sec", glm::vec2(windowWidth * 0.055f, windowHeight * 0.895f), windowWidth / 1920.0f * 0.3f, windowWidth, windowHeight);
	collapseCountdown = GUIText(fontPath, "Time until maze collapses: " + ((int)((int)collapseTime / 60) < 10 ? "0" + std::to_string((int) ((int)collapseTime / 60)) : std::to_string((int)((int)collapseTime / 60))) + ":" + ((int)((int)collapseTime % 60) < 10 ? "0" + std::to_string((int) ((int)collapseTime % 60)) : std::to_string((int)((int)collapseTime % 60))), glm::vec2(windowWidth * 0.755f, windowHeight * 0.95f), windowWidth / 1920.0f * 0.3f, windowWidth, windowHeight);
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
		batteryCountdown.updateText(std::to_string((int)batteryTime) + " sec");

		if (batteryTime <= 2.0f) {
			if ((int)(batteryTime * 100) % 2 == 0) {
				camera->setSpotLightParameters(glm::vec3(0.0f), 0.0f, 25.0f, glm::vec3(0.2f, 0.4f, 0.2f));
			}
			else {
				camera->setSpotLightParameters(glm::vec3(1.0f), 0.0f, 25.0f, glm::vec3(0.2f, 0.4f, 0.2f));
			}
		}
	}
	else {
		camera->setSpotLightParameters(glm::vec3(0.0f), 0.0f, 25.0f, glm::vec3(0.2f, 0.4f, 0.2f));
	}

	if (collapseTime - deltaT > 0.0f) {
		collapseTime -= deltaT;
		collapseCountdown.updateText("Time until maze collapses: " + ((int)((int)collapseTime / 60) < 10 ? "0" + std::to_string((int)((int)collapseTime / 60)) : std::to_string((int)((int)collapseTime / 60))) + ":" + ((int)((int)collapseTime % 60) < 10 ? "0" + std::to_string((int)((int)collapseTime % 60)) : std::to_string((int)((int)collapseTime % 60))));
	}
}

void GUI::draw() {
	battery.draw(componentShader);
	batteryStatus.draw(textShader);

	if (!(batteryTime <= 10.0f && (int)(batteryTime * 2) % 2 == 0) || batteryTime <= 0.5f) {
			batteryCountdown.draw(textShader);
	}
	
	if (!(collapseTime <= 10.0f && (int)(collapseTime * 2) % 2 == 0) || collapseTime <= 0.5f) {
		collapseCountdown.draw(textShader);
	}
}

void GUI::addBattery() {
	batteryCounter++;
	batteryStatus.updateText(std::to_string(batteryCounter) + "/10");
}

void GUI::deleteBattery() {
	if (batteryCounter > 0) {
		batteryCounter--;
		batteryStatus.updateText(std::to_string(batteryCounter) + "/10");
		batteryTime = 60.0f;
		camera->setSpotLightParameters(glm::vec3(1.0f), 0.0f, 25.0f, glm::vec3(0.2f, 0.4f, 0.2f));
	}
}
