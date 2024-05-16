#pragma once

#include <memory>

#include "Context/Driver.h"
#include "Settings/WindowSettings.h"
#include "Core/CameraController.h"

#include "Inputs/InputManager.h"

#include "Rendering/Renderer.h"

namespace Rendering
{
	class ContextCore
	{
	public:
		ContextCore(const Settings::WindowSettings& p_windowSettings, const Settings::DriverSettings& p_driverSettings);
		~ContextCore() = default;

	public:
		std::unique_ptr<Context::Device>      device;
		std::unique_ptr<Context::Window>      window;
		std::unique_ptr<Context::Driver>      driver;
		std::unique_ptr<Inputs::InputManager> inputManager;
		std::unique_ptr<Rendering::Renderer>       renderer;
	};
}
