#pragma once

#include <glm/detail/type_quat.hpp>

#include "AmberRenderer/Core/Context.h"

#include "AmberRenderer/Rendering/Rasterizer/Buffers/FrameBuffer.h"
#include "AmberRenderer/Rendering/Rasterizer/Shaders/ShadowMappingDepth.h"

namespace AmberRenderer::Core
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
		Core::Context m_context;
		
		Entities::Camera m_camera;
		Core::CameraController m_cameraController;

		glm::vec3 m_cameraPosition;
		glm::quat m_cameraRotation;

		Resources::Model* m_currentModel = nullptr;
		Rendering::Rasterizer::Shaders::ShadowMappingDepth m_shadowMapDepthShader;

		Rendering::Rasterizer::Buffers::FrameBuffer<Depth>* m_shadowDepthBuffer = nullptr;
		Rendering::Rasterizer::Buffers::FrameBuffer<RGBA8>* m_shadowBuffer = nullptr;
		Resources::Texture* m_shadowMapTexture = nullptr;

		bool m_isRunning;
	};
}
