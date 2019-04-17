#pragma once

#include <iostream>

/*!
 * Initialization File (.ini) Reader
 */
class INIReader
{
public:
	/*!
	 * Struct containing settings for the program
	 */
	struct Settings {
		unsigned int width;
		unsigned int height;
		unsigned int refresh_rate;
		float field_of_view;
		bool fullscreen;
	};
	/*!
	 * Reads from the settings.ini file and returns a Settings struct
	 */
	static Settings ReadSettings();
protected:
	/*!
	 * Vallidates the Settings struct if all important information has been set
	 */
	static bool ValidateSettings(Settings s);
};

