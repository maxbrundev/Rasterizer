#pragma once

#include <typeindex>

#include "OnyxEditor/Buffers/UniformBuffer.h"
#include "OnyxEditor/Context/IDisplay.h"
#include "OnyxEditor/Context/SDLDriver.h"

#include "OnyxEditor/Entities/Camera.h"

#include "OnyxEditor/Rendering/ARenderer.h"
#include "OnyxEditor/Rendering/Driver.h"
#include "OnyxEditor/Rendering/Features/ARenderFeature.h"
#include "OnyxEditor/Rendering/Passes/ARenderPass.h"

#include "OnyxEditor/Resources/Material.h"
#include "OnyxEditor/Resources/Model.h"

#include "OnyxEditor/SceneSystem/AScene.h"
#include "Debug/DepthBufferVisualizer.h"
#include "Debug/EDebugViewMode.h"

namespace OnyxEditor::Rendering
{
	template<typename T>
	concept RenderFeature = std::is_base_of_v<OnyxEditor::Rendering::Features::ARenderFeature, T>;

	template<typename T>
	concept RenderPass = std::is_base_of_v<OnyxEditor::Rendering::Passes::ARenderPass, T>;

	class Renderer : public ARenderer
	{
	public:
		Renderer(Driver& p_driver, Context::IDisplay& p_display);
		~Renderer() override = default;

		virtual void BeginFrame(const FrameInfo& p_frameDescriptor) override;
		virtual void DrawFrame(SceneSystem::AScene* p_scene) override;
		virtual void EndFrame() override;

		virtual void Draw(RenderState p_pso, const Entities::Drawable& p_drawable) override;

		void SetClearColor(float p_red, float p_green, float p_blue, float p_alpha);
		void SetSamples(uint8_t p_samples) const;

		void DisplayPresent() const;
		void DisplayClear() const;

		template<RenderFeature T, typename ... Args>
		T& AddFeature(Args&&... p_args);

		template<RenderFeature T>
		bool RemoveFeature();

		template<RenderFeature T>
		T& GetFeature() const;

		template<RenderFeature T>
		bool HasFeature() const;

		template<RenderPass T, typename ... Args>
		T& AddPass(const std::string& p_name, uint32_t p_order, Args&&... p_args);

		template<RenderPass T>
		T& GetPass(const std::string& p_name) const;

		void InitializeDebugMaterials();
		Resources::Material* GetDebugMaterialForCurrentMode() const;

		void SetDebugViewMode(Debug::EDebugViewMode p_debugViewMode);
		Debug::EDebugViewMode GetDebugViewMode() { return m_currentMode; }
		const char* GetViewModeName(Debug::EDebugViewMode mode)
		{
			switch (mode)
			{
			case Debug::EDebugViewMode::STANDARD: return "Standard";
			case Debug::EDebugViewMode::WIREFRAME: return "Wireframe";
			case Debug::EDebugViewMode::NORMALS: return "World Normals";
			case Debug::EDebugViewMode::UV_COORDINATES: return "UV Coordinates";
			case Debug::EDebugViewMode::DEPTH_BUFFER: return "Depth Buffer";
			case Debug::EDebugViewMode::CHECKERBOARD_UV: return "UV Checkerboard";
			default: return "Unknown";
			}
		}

	private:
		void RenderDepthVisualization(SceneSystem::AScene* p_scene);
		void RenderNormalPasses(SceneSystem::AScene* p_scene);

	private:
		Context::IDisplay& m_display;

		std::unordered_map<std::type_index, std::unique_ptr<Features::ARenderFeature>> m_features;
		std::multimap<uint32_t, std::pair<std::string, std::unique_ptr<Passes::ARenderPass>>> m_passes;

		std::unique_ptr<Debug::DepthBufferVisualizer> m_depthVisualizer;

		std::unique_ptr<Resources::Material> m_debugUVMaterial;
		std::unique_ptr<Resources::Material> m_debugNormalsMaterial;
		std::unique_ptr<Resources::Material> m_debugDepthMaterial;

		Debug::EDebugViewMode m_currentMode;
	};
}

#include "OnyxEditor/Rendering/Renderer.inl"