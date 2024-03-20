#pragma once

#include "Core/ContextCore.h"

#include "Settings/WindowSettings.h"
#include "Settings/DriverSettings.h"

namespace Core
{
	class Application
	{
	public:
		Application(const Settings::WindowSettings& p_windowSettings, const Settings::DriverSettings& p_driverSettings);
		~Application() = default;

		void Initialize();
		void Run();

		bool IsRunning() const;

	private:
		ContextCore m_context;
		
		Entities::Camera m_camera;
		Core::CameraController m_cameraController;

		glm::vec3 m_cameraPosition;

		bool m_isRunning;
	};
}
