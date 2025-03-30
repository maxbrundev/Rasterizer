#pragma once

#include <memory>

#include "AmberRenderer/Context/SDLDriver.h"

#include "AmberRenderer/Core/CameraController.h"

#include "AmberRenderer/Inputs/InputManager.h"

#include "AmberRenderer/Rendering/Renderer.h"

namespace AmberRenderer::Core
{
	class Context
	{
	public:
		Context();
		~Context() = default;

	public:
		std::unique_ptr<AmberRenderer::Context::Device> Device;
		std::unique_ptr<AmberRenderer::Context::Window> Window;
		std::unique_ptr<AmberRenderer::Context::SDLDriver> SDLDriver;
		std::unique_ptr<Rendering::Driver> Driver;
		std::unique_ptr<Inputs::InputManager> InputManager;
		std::unique_ptr<Rendering::Renderer> Renderer;
	};
}
