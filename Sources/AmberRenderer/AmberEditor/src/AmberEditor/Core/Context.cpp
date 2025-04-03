#include "AmberEditor/Core/Context.h"

#include "AmberEditor/Tools/Globals/ServiceLocator.h"

AmberEditor::Core::Context::Context()
{
	Device = std::make_unique<AmberEditor::Context::Device>();

	AmberEditor::Context::Settings::WindowSettings windowSettings;
	windowSettings.title = "Amber Renderer";
	windowSettings.width = 800;
	windowSettings.height = 600;

	Window = std::make_unique<AmberEditor::Context::Window>(*Device, windowSettings);

	AmberEditor::Context::Settings::SDLDriverSettings driverSettings;
	driverSettings.vsync = false;

	SDLDriver = std::make_unique<AmberEditor::Context::SDLDriver>(*Window, driverSettings);

	InputManager = std::make_unique<Inputs::InputManager>(*Device);

	Driver = std::make_unique<Rendering::Driver>();

	Renderer = std::make_unique<Rendering::Renderer>(*SDLDriver, *Driver);

	Tools::Globals::ServiceLocator::Provide(*Window);
	Tools::Globals::ServiceLocator::Provide(*InputManager);
	Tools::Globals::ServiceLocator::Provide(*Renderer);
}
