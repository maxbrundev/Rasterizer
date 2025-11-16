#pragma once

#include <glm/glm.hpp>

#include "OnyxEditor/Buffers/UniformBuffer.h"
#include "OnyxEditor/Context/SDLDriver.h"
#include "OnyxEditor/Entities/Drawable.h"

#include "OnyxEditor/Rendering/Driver.h"
#include "OnyxEditor/Rendering/FrameInfo.h"
#include "OnyxEditor/Rendering/RenderState.h"

#include "OnyxEditor/Resources/IMesh.h"
#include "OnyxEditor/Resources/Texture.h"

#include "OnyxEditor/SceneSystem/AScene.h"
#include "Debug/FrameDebugger.h"

namespace OnyxEditor::Rendering
{
	class ARenderer
	{
	public:
		ARenderer(Driver& p_driver);
		virtual ~ARenderer();

		void SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const;

		virtual void Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer, const glm::vec4& p_color = glm::vec4(0.0f));

		virtual void BeginFrame(const FrameInfo& p_frameDescriptor);
		virtual void DrawFrame(SceneSystem::AScene* p_scene) = 0;
		virtual void EndFrame();

		virtual void Draw(RenderState p_renderState, const Entities::Drawable& p_drawable);

		void RenderUnitQuad(Resources::Material* shader);

		void RegisterModelMatrixSender(std::function<void(glm::mat4)> p_modelMatrixSender);
		void RegisterUserMatrixSender(std::function<void(glm::mat4)> p_userMatrixSender);
		void UpdateViewProjectionMatrices(const glm::mat4& p_viewMatrix, const glm::mat4& p_projectionMatrix, const glm::vec3& p_cameraPosition);
		void UpdateTime(float p_time);

		uint32_t CaptureFrameToTexture(uint32_t width, uint32_t height);

		void InitializeFrameDebugger(Context::SDLDriver& sdlRenderer, uint32_t width, uint32_t height);

		Debug::FrameDebugger* GetFrameDebugger() const;
		const FrameInfo& GetFrameDescriptor() const { return m_frameDescriptor; }

		Driver& GetDriver() { return m_driver; }

	private:
		void InitializeResources();

	protected:
		Driver& m_driver;
		Resources::Texture* m_emptyTexture;
		Resources::Mesh* m_unitQuad;
		FrameInfo m_frameDescriptor;
		std::vector<uint32_t> m_debugCaptureTextures;

		std::unique_ptr<Debug::FrameDebugger> m_frameDebugger;

		std::unique_ptr<Buffers::UniformBuffer> m_engineUBO;
		std::function<void(glm::mat4)> m_modelMatrixSender;
		std::function<void(glm::mat4)> m_userMatrixSender;
	};
}
