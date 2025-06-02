#include "AmberEditor/Core/Context.h"

#include "AmberEditor/Context/SDLDisplay.h"

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

	Display = std::make_unique<AmberEditor::Context::SDLDisplay>(*SDLDriver, 800, 600);

	Rendering::RenderState defaultRenderState;
	defaultRenderState.MultiSample = false;

	Driver = std::make_unique<Rendering::Driver>(Rendering::Settings::DriverSettings{ defaultRenderState });
	
	Renderer = std::make_unique<Rendering::Renderer>(*Driver, *Display);

	InputManager = std::make_unique<Inputs::InputManager>(*Device);

	Tools::Globals::ServiceLocator::Provide(*Window);
	Tools::Globals::ServiceLocator::Provide(*InputManager);
	Tools::Globals::ServiceLocator::Provide(*Renderer);
	Tools::Globals::ServiceLocator::Provide(ModelManager);
	Tools::Globals::ServiceLocator::Provide(TextureManager);
	Tools::Globals::ServiceLocator::Provide(ShaderManager);
}
