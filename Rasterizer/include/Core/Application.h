#pragma once

#include "Core/ContextCore.h"

#include "Context/Settings/WindowSettings.h"
#include "Context/Settings/DriverSettings.h"

namespace Core
{
	class Application
	{
	public:
		Application(const Context::Settings::WindowSettings& p_windowSettings, const Context::Settings::DriverSettings& p_driverSettings);
		~Application() = default;

		void Initialize();
		void Run();

		bool IsRunning() const;

	private:
		Core::ContextCore m_context;
		
		Entities::Camera m_camera;
		Core::CameraController m_cameraController;

		glm::vec3 m_cameraPosition;

		bool m_isRunning;
	};
}
