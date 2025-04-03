#pragma once

#include <glm/detail/type_quat.hpp>

#include "AmberEditor/Core/Context.h"

namespace AmberEditor::Core
{
	class Application
	{
	public:
		Application();
		~Application();

		void Initialize();
		void Run();

		bool IsRunning() const;

	private:
		Context m_context;
		
		Entities::Camera m_camera;
		CameraController m_cameraController;

		glm::vec3 m_cameraPosition;
		glm::quat m_cameraRotation;

		Resources::Model* m_currentModel = nullptr;
	
		Resources::Shader* m_shadowDepthShaderResource;

		bool m_isRunning;
	};
}
