#pragma once

#include <memory>

#include "AmberEditor/Context/SDLDriver.h"

#include "AmberEditor/Core/CameraController.h"

#include "AmberEditor/Inputs/InputManager.h"

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
		std::unique_ptr<Rendering::Driver> Driver;
		std::unique_ptr<Inputs::InputManager> InputManager;
		std::unique_ptr<Rendering::Renderer> Renderer;
	};
}
