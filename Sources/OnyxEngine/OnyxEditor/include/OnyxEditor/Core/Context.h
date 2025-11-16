#pragma once

#include <memory>

#include "OnyxEditor/Rendering/Driver.h"

#include "OnyxEditor/Context/SDLDriver.h"
#include "OnyxEditor/Context/IDisplay.h"

#include "OnyxEditor/Core/CameraController.h"

#include "OnyxEditor/Inputs/InputManager.h"
#include "OnyxEditor/Managers/ModelManager.h"
#include "OnyxEditor/Managers/ShaderManager.h"
#include "OnyxEditor/Managers/TextureManager.h"

#include "OnyxEditor/Rendering/Renderer.h"
#include "OnyxEditor/SceneSystem/SceneManager.h"
#include "OnyxEditor/UI/Core/UIManager.h"

namespace OnyxEditor::Core
{
	class Context
	{
	public:
		Context();
		~Context() = default;

	public:
		const std::string engineAssetsPath;
		const std::string editorAssetsPath;
		const std::string projectAssetsPath;

		std::unique_ptr<OnyxEditor::Context::Device> Device;
		std::unique_ptr<OnyxEditor::Context::Window> Window;
		std::unique_ptr<OnyxEditor::Context::SDLDriver> SDLDriver;
		std::unique_ptr<OnyxEditor::Context::IDisplay> Display;
		std::unique_ptr<OnyxEditor::Rendering::Driver> Driver;
		std::unique_ptr<OnyxEditor::Rendering::Renderer> Renderer;
		std::unique_ptr<OnyxEditor::Inputs::InputManager> InputManager;
		std::unique_ptr<OnyxEditor::UI::Core::UIManager> UIManager;


		OnyxEditor::Managers::ModelManager ModelManager;
		OnyxEditor::Managers::TextureManager TextureManager;
		OnyxEditor::Managers::ShaderManager ShaderManager;

		OnyxEditor::SceneSystem::SceneManager SceneManager;
	};
}
