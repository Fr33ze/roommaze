#include "INIReader.h"

INIReader::Settings INIReader::ReadSettings()
{
	Settings s = {};
	FILE *inifile;
	errno_t err = fopen_s(&inifile, "assets/settings.ini", "r");
	if (err != 0) {
		std::cout << "Unable to open file assets/settings.ini" << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}

	while (1) {
		char lineheader[64];
		//read until the character '='
		int res = fscanf_s(inifile, "%[^=\n]\n", lineheader, sizeof(lineheader));
		if (res == EOF) {
			break;
		}
		else {
			if (strcmp(lineheader, "width") == 0) {
				fscanf_s(inifile, "=%d\n", &s.width);
			}
			else if (strcmp(lineheader, "height") == 0) {
				fscanf_s(inifile, "=%d\n", &s.height);
			}
			else if (strcmp(lineheader, "refresh_rate") == 0) {
				fscanf_s(inifile, "=%d\n", &s.refresh_rate);
			}
			else if (strcmp(lineheader, "fov") == 0) {
				fscanf_s(inifile, "=%f\n", &s.field_of_view);
			}
			else if (strcmp(lineheader, "fullscreen") == 0) {
				unsigned int fullscreenvalue;
				fscanf_s(inifile, "=%d\n", &fullscreenvalue);
				fullscreenvalue ? s.fullscreen = true : s.fullscreen = false;
			}
		}
	}
	//Stop if important setting is missing
	if (!ValidateSettings(s)) {
		std::cout << "Settings file assets/settings.ini is corrupted!" << std::endl;
		std::cout << "Press ENTER to close this window." << std::endl;
		getchar();
		exit(-1);
	}
	return s;
}

bool INIReader::ValidateSettings(Settings s)
{
	return !(s.width == 0 || s.height == 0 || s.refresh_rate == 0 || s.field_of_view == 0.0f);
}