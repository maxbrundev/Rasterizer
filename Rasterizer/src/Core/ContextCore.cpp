#include "Core/ContextCore.h"

#include "Tools/Globals/ServiceLocator.h"

Rendering::ContextCore::ContextCore(const Settings::WindowSettings& p_windowSettings, const Settings::DriverSettings& p_driverSettings)
{
	device = std::make_unique<Context::Device>();
	window = std::make_unique<Context::Window>(*device, p_windowSettings);
	driver = std::make_unique<Context::Driver>(*window, p_driverSettings);

	inputManager = std::make_unique<Inputs::InputManager>(*device);

	renderer = std::make_unique<Renderer>(*driver, *window);

	Tools::Globals::ServiceLocator::Provide(*window);
	Tools::Globals::ServiceLocator::Provide(*inputManager);
}
