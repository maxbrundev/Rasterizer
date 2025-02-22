#include "Core/ContextCore.h"

#include "Tools/Globals/ServiceLocator.h"

Core::ContextCore::ContextCore(const Context::Settings::WindowSettings& p_windowSettings, const Context::Settings::DriverSettings& p_driverSettings)
{
	device = std::make_unique<Context::Device>();
	window = std::make_unique<Context::Window>(*device, p_windowSettings);
	driver = std::make_unique<Context::SDLDriver>(*window, p_driverSettings);

	inputManager = std::make_unique<Inputs::InputManager>(*device);

	renderDriver = std::make_unique<Rendering::Driver>();

	renderer = std::make_unique<Rendering::Renderer>(*driver, *renderDriver, *window);

	Tools::Globals::ServiceLocator::Provide(*window);
	Tools::Globals::ServiceLocator::Provide(*inputManager);

	
}
