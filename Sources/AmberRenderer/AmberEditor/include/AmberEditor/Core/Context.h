#pragma once

#include <memory>

#include "AmberEditor/Rendering/Driver.h"

#include "AmberEditor/Context/SDLDriver.h"
#include "AmberEditor/Context/IDisplay.h"

#include "AmberEditor/Core/CameraController.h"

#include "AmberEditor/Inputs/InputManager.h"
#include "AmberEditor/Managers/ModelManager.h"
#include "AmberEditor/Managers/ShaderManager.h"
#include "AmberEditor/Managers/TextureManager.h"

#include "AmberEditor/Rendering/Renderer.h"

namespace AmberEditor::Core
{
	class Context
	{
	public:
		Context();
		~Context() = default;

	public:
		std::unique_ptr<AmberEditor::Context::Device> Device;
		std::unique_ptr<AmberEditor::Context::Window> Window;
		std::unique_ptr<AmberEditor::Context::SDLDriver> SDLDriver;
		std::unique_ptr<AmberEditor::Context::IDisplay> Display;
		std::unique_ptr<AmberEditor::Rendering::Driver> Driver;
		std::unique_ptr<AmberEditor::Rendering::Renderer> Renderer;
		std::unique_ptr<AmberEditor::Inputs::InputManager> InputManager;

		AmberEditor::Managers::ModelManager ModelManager;
		AmberEditor::Managers::TextureManager TextureManager;
		AmberEditor::Managers::ShaderManager ShaderManager;
	};
}
