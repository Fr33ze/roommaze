#include "GUI.h"

GUI::GUI(int windowWidth, int windowHeight, int startingBatteries)
	: windowWidth(windowWidth), windowHeigth(windowHeight), time(0.0f), batteryCounter(startingBatteries) {

	componentShader = std::make_shared<Shader>("assets/shaders/guiComponent.vert", "assets/shaders/guiComponent.frag");
	textShader = std::make_shared<Shader>("assets/shaders/guiText.vert", "assets/shaders/guiText.frag");
	
	battery = GUIComponent("assets/gui/battery.png", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.95f, 0.88f, 0.0f)), glm::vec3(1.0f / windowWidth * 100, 1.0f / windowHeight * 100, 1.0f)));

	batteryStatus = GUIText("assets/gui/ArialCE.ttf", std::to_string(startingBatteries) + "/10", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.88f, 0.87f, 0.0f)), glm::vec3(1.0f / windowWidth * 20, 1.0f / windowHeigth * 20, 1.0f)));
	batteryCountdown = GUIText("assets/gui/ArialCE.ttf", "60 sec", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.95f, 0.7f, 0.0f)), glm::vec3(1.0f / windowWidth * 20, 1.0f / windowHeigth * 20, 1.0f)));
	mazeCountdown = GUIText("assets/gui/ArialCE.ttf", "60 sec", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.85f, 0.95f, 0.0f)), glm::vec3(1.0f / windowWidth * 20, 1.0f / windowHeigth * 20, 1.0f)));
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
	mazeCountdown.updateText(std::to_string(60 - (int)time) + " sec");
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
