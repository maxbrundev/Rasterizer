#define SDL_MAIN_HANDLED

#include "Core/Application.h"

int main()
{
	Context::Settings::WindowSettings windowSettings;
	windowSettings.title = "Rasterizer";
	windowSettings.width  = 800;
	windowSettings.height = 600;

	Context::Settings::DriverSettings driverSettings;
	driverSettings.vsync = true;

	Core::Application application(windowSettings, driverSettings);
	application.Initialize();
	application.Run();

	return 0;
}
