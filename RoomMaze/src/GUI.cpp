#include "GUI.h"

GUI::GUI(int windowWidth, int windowHeight, int startingBatteries)
	: time(0.0f), batteryCounter(startingBatteries) {

	componentShader = std::make_shared<Shader>("assets/shaders/gui.vert", "assets/shaders/guiComponent.frag");
	textShader = std::make_shared<Shader>("assets/shaders/gui.vert", "assets/shaders/guiText.frag");
	
	battery = GUIComponent("assets/gui/battery.png", glm::vec2(windowWidth * 0.002f, windowHeight * 0.98f), 0.05f, windowWidth, windowHeight);

	std::string fontPath = "assets/gui/Roboto-Regular.ttf";

	batteryStatus = GUIText(fontPath, std::to_string(startingBatteries) + "/10", glm::vec2(windowWidth * 0.055f, windowHeight * 0.94f), windowWidth / 1920.0f * 0.5f, windowWidth, windowHeight);
	batteryCountdown = GUIText(fontPath, "60 sec", glm::vec2(windowWidth * 0.055f, windowHeight * 0.9f), windowWidth / 1920.0f * 0.3f, windowWidth, windowHeight);
	mazeCountdown = GUIText(fontPath, "Time until maze collapses: 120 sec", glm::vec2(windowWidth * 0.74f, windowHeight * 0.95f), windowWidth / 1920.0f * 0.3f, windowWidth, windowHeight);
}

GUI::GUI() {

}

GUI::~GUI() {

}

void GUI::destroy() {
	
}

void GUI::updateTime(float deltaT) {
	time += deltaT;
	batteryCountdown.updateText(std::to_string(60 - (int)time) + " sec");
	mazeCountdown.updateText("Time until maze collapses: " + std::to_string(120 - (int)time) + " sec");
}

void GUI::draw() {
	battery.draw(componentShader);
	batteryStatus.draw(textShader);
	batteryCountdown.draw(textShader);
	mazeCountdown.draw(textShader);
}

void GUI::addBattery() {
	batteryCounter++;
	batteryStatus.updateText(std::to_string(batteryCounter) + "/10");
}

void GUI::deleteBattery() {
	batteryCounter--;
	batteryStatus.updateText(std::to_string(batteryCounter) + "/10");
}
