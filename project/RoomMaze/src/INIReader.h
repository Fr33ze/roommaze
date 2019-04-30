#pragma once

#include <iostream>

/**
 * Initialization File (.ini) Reader
 */
class INIReader
{
public:
	/**
	 * contains settings for the application
	 */
	struct Settings {
		unsigned int width;
		unsigned int height;
		unsigned int refresh_rate;
		float field_of_view;
		bool fullscreen;
	};
	/**
	 * Reads from the settings.ini file and returns a Settings struct
	 */
	static Settings ReadSettings();
protected:
	/**
	 * Vallidates the Settings struct if all important information has been set
	 *
	 * @param s: settings struct to be validated
	 */
	static bool ValidateSettings(Settings s);
};

