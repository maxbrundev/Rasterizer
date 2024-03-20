#define SDL_MAIN_HANDLED

#include "Core/Application.h"

int main()
{
	Settings::WindowSettings windowSettings;
	windowSettings.title = "Rasterizer";
	windowSettings.width = 1920;
	windowSettings.height = 1080;
	
	Settings::DriverSettings driverSettings;
	driverSettings.vsync = true;

	Core::Application application(windowSettings, driverSettings);
	application.Initialize();
	application.Run();

	return 0;
}
