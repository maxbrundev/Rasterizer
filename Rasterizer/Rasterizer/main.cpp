#define SDL_MAIN_HANDLED

#include "Core/Application.h"

int main()
{
	Settings::WindowSettings windowSettings;
	windowSettings.title = "Rasterizer";
	windowSettings.width = 800;
	windowSettings.height = 600;
	
	Settings::DriverSettings driverSettings;
	driverSettings.vsync = true;

	Rendering::Application application(windowSettings, driverSettings);
	application.Initialize();
	application.Run();

	return 0;
}
