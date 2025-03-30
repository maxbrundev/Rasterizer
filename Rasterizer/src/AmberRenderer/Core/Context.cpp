#include "AmberRenderer/Core/Context.h"

#include "AmberRenderer/Tools/Globals/ServiceLocator.h"

AmberRenderer::Core::Context::Context()
{
	Device = std::make_unique<AmberRenderer::Context::Device>();

	AmberRenderer::Context::Settings::WindowSettings windowSettings;
	windowSettings.title = "Amber Renderer";
	windowSettings.width = 800;
	windowSettings.height = 600;

	Window = std::make_unique<AmberRenderer::Context::Window>(*Device, windowSettings);

	AmberRenderer::Context::Settings::SDLDriverSettings driverSettings;
	driverSettings.vsync = false;

	SDLDriver = std::make_unique<AmberRenderer::Context::SDLDriver>(*Window, driverSettings);

	InputManager = std::make_unique<Inputs::InputManager>(*Device);

	Driver = std::make_unique<Rendering::Driver>();

	Renderer = std::make_unique<Rendering::Renderer>(*SDLDriver, *Driver);

	Tools::Globals::ServiceLocator::Provide(*Window);
	Tools::Globals::ServiceLocator::Provide(*InputManager);
	Tools::Globals::ServiceLocator::Provide(*Renderer);
}
