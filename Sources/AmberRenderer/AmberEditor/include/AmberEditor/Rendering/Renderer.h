#pragma once

#include <typeindex>

#include "AmberEditor/Context/IDisplay.h"
#include "AmberEditor/Context/SDLDriver.h"

#include "AmberEditor/Entities/Camera.h"

#include "AmberEditor/Rendering/ARenderer.h"
#include "AmberEditor/Rendering/Driver.h"
#include "AmberEditor/Rendering/Features/ARenderFeature.h"
#include "AmberEditor/Rendering/Passes/ARenderPass.h"

#include "AmberEditor/Resources/Material.h"
#include "AmberEditor/Resources/Model.h"

#include "AmberEditor/SceneSystem/AScene.h"

namespace AmberEditor::Rendering
{
	template<typename T>
	concept RenderFeature = std::is_base_of_v<AmberEditor::Rendering::Features::ARenderFeature, T>;

	template<typename T>
	concept RenderPass = std::is_base_of_v<AmberEditor::Rendering::Passes::ARenderPass, T>;

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

	private:
		void OnResize(uint16_t p_width, uint16_t p_height);

	private:
		Context::IDisplay& m_display;

		std::unordered_map<std::type_index, std::unique_ptr<Features::ARenderFeature>> m_features;
		std::multimap<uint32_t, std::pair<std::string, std::unique_ptr<Passes::ARenderPass>>> m_passes;
	};
}

#include "AmberEditor/Rendering/Renderer.inl"