#include "GUI.h"

GUI::GUI(int windowWidth, int windowHeight)
	: batteryTime(60.0f), windowWidth(windowWidth) {

	componentShader = std::make_shared<Shader>("assets/shaders/quad.vert", "assets/shaders/guiComponent.frag");
	textShader = std::make_shared<Shader>("assets/shaders/quad.vert", "assets/shaders/guiText.frag");
	
	battery = GUIComponent("assets/gui/battery.png", glm::vec2(0.0f, windowHeight * 0.99f), 0.05f, windowWidth, windowHeight, false);
	keyComp = GUIComponent("assets/gui/key.png", glm::vec2(windowWidth * 0.005f, windowHeight * 0.9f), 0.05f, windowWidth, windowHeight, false);
	resistanceComp = GUIComponent("assets/gui/resistance.png", glm::vec2(windowWidth * 0.004f, windowHeight * 0.81f), 0.05f, windowWidth, windowHeight, false);
	buttonComp = GUIComponent("assets/gui/button.png", glm::vec2(windowWidth * 0.004f, windowHeight * 0.81f), 0.05f, windowWidth, windowHeight, false);

	startScreen = GUIComponent("assets/gui/start_screen.jpg", glm::vec2(0.0f, windowHeight), 1.0f, windowWidth, windowHeight, true);
	gameOverScreen = GUIComponent("assets/gui/game_over_screen.jpg", glm::vec2(0.0f, windowHeight), 1.0f, windowWidth, windowHeight, true);
	endScreen = GUIComponent("assets/gui/end_screen.jpg", glm::vec2(0.0f, windowHeight), 1.0f, windowWidth, windowHeight, true);

	std::string fontPath = "assets/gui/Roboto-Regular.ttf";

	batteryStatus = GUIText(fontPath, "x" + std::to_string(batteries), glm::vec2(windowWidth * 0.045f, windowHeight * 0.955f), windowWidth / 1920.0f * 0.3f, 0.75f, windowWidth, windowHeight);
	batteryCountdown = GUIText(fontPath, std::to_string((int)(batteryTime / 0.6f)) + " %", glm::vec2(windowWidth * 0.045f, windowHeight * 0.915f), windowWidth / 1920.0f * 0.4f, 0.75f, windowWidth, windowHeight);
	infoText = GUIText(fontPath, "", glm::vec2(windowWidth * 0.015f, windowHeight * 0.8f), windowWidth / 1920.0f * 0.5f, 0.75f, windowWidth, windowHeight);

	ghostBuffer = alutCreateBufferFromFile("assets/audio/ghost_attack.wav");
	reloadBatteryBuffer = alutCreateBufferFromFile("assets/audio/battery_reload.wav");
	flickerOnBuffer = alutCreateBufferFromFile("assets/audio/flashlight_on.wav");
	flickerOffBuffer = alutCreateBufferFromFile("assets/audio/flashlight_off.wav");

	alGenSources(1, &batterySource);
	alSourcef(batterySource, AL_PITCH, 1);
	alSourcef(batterySource, AL_GAIN, 1);
	alSourcei(batterySource, AL_BUFFER, reloadBatteryBuffer);
	alSourcei(batterySource, AL_SOURCE_RELATIVE, AL_TRUE);

	alGenSources(1, &flashlightSource);
	alSourcef(flashlightSource, AL_PITCH, 1);
	alSourcef(flashlightSource, AL_GAIN, 1);
	alSourcei(flashlightSource, AL_SOURCE_RELATIVE, AL_TRUE);

	alGenSources(1, &audioSource);
	alSourcef(audioSource, AL_PITCH, 1);
	alSourcef(audioSource, AL_GAIN, 0.3f);
	alSourcei(audioSource, AL_BUFFER, ghostBuffer);

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
		batteryCountdown.updateText(std::to_string((int)(batteryTime / 0.6f)) + " %");

		// if battery has 3 seconds left, simulate flickering
		if (batteryTime <= 3.f) {
			// if flickeringCounter < 0 lamp is off
			if (flickeringCounter < 0) {
				// if flickeringCounter + deltaT >= 0 calculate new value
				if (flickeringCounter + deltaT >= 0) {
					std::random_device rd;
					std::mt19937 eng(rd());
					std::uniform_real_distribution<> distr(-0.1f, 0.3f);

					flickeringCounter = distr(eng);

					// if light status changed
					if (flickeringCounter >= 0) {
						camera->turnSpotlightOn();
						// possible sound comes here bitches ... hehe ;D
						alSourcei(flashlightSource, AL_BUFFER, flickerOnBuffer);
						alSourcePlay(flashlightSource);
					}
				}
				else
					flickeringCounter += deltaT;
			}
			// if flickeringCounter >= 0 lamp is on
			else {
				// if flickeringCounter - deltaT < 0 calculate new value
				if (flickeringCounter - deltaT < 0) {
					std::random_device rd;
					std::mt19937 eng(rd());
					std::uniform_real_distribution<> distr(-0.1f, 0.3f);

					flickeringCounter = distr(eng);

					// if light status changed
					if (flickeringCounter < 0) {
						camera->turnSpotlightOff();
						// possible sound comes here bitches ... hehe ;D
						alSourcei(flashlightSource, AL_BUFFER, flickerOffBuffer);
						alSourcePlay(flashlightSource);
					}
				}
				else
					flickeringCounter -= deltaT;
			}
		}

		// reset overtime when the player inserts a new battery
		overtime = 0;
	}
	else if (!endScreenIsEnabled) {
		camera->turnSpotlightOff();

		overtime += deltaT;
		if (!gameoveraudioplayed && overtime > 5) {
			alSourcePlay(audioSource);
			gameoveraudioplayed = true;
		} else if (overtime > 9 && !gameOverScreenIsEnabled) {
			showGameOverScreen(true);
		}
	}
}

void GUI::draw(float sceneBrightness) {
	if (startScreenIsEnabled)
		startScreen.draw(componentShader, sceneBrightness);
	else if (gameOverScreenIsEnabled)
		gameOverScreen.draw(componentShader, sceneBrightness);
	else if (endScreenIsEnabled)
		endScreen.draw(componentShader, sceneBrightness);
	else {
		battery.draw(componentShader, sceneBrightness);
		batteryStatus.draw(textShader, sceneBrightness);

		if (!(batteryTime <= 10.0f && (int)(batteryTime * 2) % 2 == 0) || batteryTime <= 0.5f) {
			batteryCountdown.draw(textShader, sceneBrightness);
		}

		if (key)
			keyComp.draw(componentShader, sceneBrightness);

		if (resistance)
			resistanceComp.draw(componentShader, sceneBrightness);
		else if (button)
			buttonComp.draw(componentShader, sceneBrightness);

		infoText.draw(textShader, sceneBrightness);
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

void GUI::addBattery(bool isCheater) {
	if (isCheater) {
		batteries = 99;
		batteryStatus.updateText("x" + std::to_string(batteries));
	}
	else {
		batteries++;
		batteryStatus.updateText("x" + std::to_string(batteries));
	}
}

void GUI::removeBattery()
{
	if (batteries > 0 && !endScreenIsEnabled) {
		alSourcePlay(batterySource);
		batteries--;
		batteryStatus.updateText("x" + std::to_string(batteries));
		batteryTime = 60.0f;
		extern Camera *camera;
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
