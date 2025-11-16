#include "OnyxEditor/Core/Context.h"

#include <filesystem>

#include "OnyxEditor/Context/SDLDisplay.h"
#include "OnyxEditor/Data/EditorConstants.h"

#include "OnyxEditor/Tools/Globals/ServiceLocator.h"

OnyxEditor::Core::Context::Context() :
engineAssetsPath(std::filesystem::canonical("Data\\Engine").string() + "\\"),
editorAssetsPath(Data::EditorConstants::EDITOR_ASSETS_PATH),
projectAssetsPath(Data::EditorConstants::PROJECT_ASSETS_PATH)
{
	Device = std::make_unique<OnyxEditor::Context::Device>();

	OnyxEditor::Context::Settings::WindowSettings windowSettings;
	windowSettings.title = "Onyx Engine";
	windowSettings.width = 960;
	windowSettings.height = 540;

	Window = std::make_unique<OnyxEditor::Context::Window>(*Device, windowSettings);

	OnyxEditor::Context::Settings::SDLDriverSettings driverSettings;
	driverSettings.vsync = false;

	SDLDriver = std::make_unique<OnyxEditor::Context::SDLDriver>(*Window, driverSettings);

	Display = std::make_unique<OnyxEditor::Context::SDLDisplay>(*SDLDriver, 960, 540);

	Rendering::RenderState defaultRenderState;
	defaultRenderState.MultiSample = false;

	Driver = std::make_unique<Rendering::Driver>(Rendering::Settings::DriverSettings{ defaultRenderState });
	
	Renderer = std::make_unique<Rendering::Renderer>(*Driver, *Display);

	InputManager = std::make_unique<Inputs::InputManager>(*Device);

	std::filesystem::create_directories(std::string(getenv("APPDATA")) + "\\OnyxEditor\\");

	Managers::ModelManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);
	Managers::TextureManager::ProvideAssetPaths(projectAssetsPath, engineAssetsPath);

	UIManager = std::make_unique<UI::Core::UIManager>(Window->GetSDLWindow(), SDLDriver->GetRenderer());
	UIManager->LoadFont("Ruda_Small", Data::EditorConstants::EDITOR_FONT_PATH, Data::EditorConstants::EDITOR_FONT_SIZE_SMALL);
	UIManager->LoadFont("Ruda_Medium", Data::EditorConstants::EDITOR_FONT_PATH, Data::EditorConstants::EDITOR_FONT_SIZE_MEDIUM);
	UIManager->LoadFont("Ruda_Big", Data::EditorConstants::EDITOR_FONT_PATH, Data::EditorConstants::EDITOR_FONT_SIZE_BIG);
	UIManager->UseFont("Ruda_Small");
	UIManager->SetEditorLayoutSaveFilename(std::string(getenv("APPDATA")) + "\\OnyxEditor\\layout.ini");
	UIManager->SetEditorLayoutAutosaveFrequency(60.0f);
	UIManager->EnableEditorLayoutSave(true);
	
	if (!std::filesystem::exists(std::string(getenv("APPDATA")) + "\\OnyxEditor\\layout.ini"))
	{
		UIManager->LoadLayout("Config\\layout.ini");
	}

	Tools::Globals::ServiceLocator::Provide(*Window);
	Tools::Globals::ServiceLocator::Provide(*InputManager);
	Tools::Globals::ServiceLocator::Provide(*Renderer);
	Tools::Globals::ServiceLocator::Provide(ModelManager);
	Tools::Globals::ServiceLocator::Provide(TextureManager);
	Tools::Globals::ServiceLocator::Provide(ShaderManager);
	Tools::Globals::ServiceLocator::Provide<UI::Core::UIManager>(*UIManager);
}
