#pragma once

#include "Context/Driver.h"

#include "Rendering/Rasterizer.h"

#include "Rendering/Settings/EDrawMode.h"
#include "Rendering/Settings/ECullFace.h"

#include "Rendering/AShader.h"
#include "Resources/Model.h"
#include "Resources/Texture.h"

namespace Rendering
{
	class Renderer
	{
	public:
		Renderer(Context::Driver& p_driver, Context::Window& p_window);
		~Renderer();

		void Clear(const Data::Color& p_color) const;
		void ClearDepth() const;
		void Draw(Settings::EDrawMode p_drawMode, Resources::Model& p_model, Resources::Material* p_defaultMaterial);
		void DrawMesh(Settings::EDrawMode p_drawMode, Resources::Mesh& p_mesh, const Resources::Material& p_material);
		void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, AShader& p_shader, const Data::Color& p_color);
		void Render() const;
		void Clear() const;
		void SetSamples(uint8_t p_samples) const;

		uint8_t FetchState() const;
		void ApplyState(uint8_t p_state);
		void SetState(uint8_t p_state);

		void SetDepthTest(bool p_value);
		void SetDepthWrite(bool p_value);
		void SetCull(bool p_value);

		void SetCullFace(Settings::ECullFace p_cullFace);

		SDL_Renderer* GetSDLRenderer() const;

	private:
		Context::Driver& m_driver;
		std::unique_ptr<Rasterizer> m_rasterizer;

		uint8_t m_state;
		uint8_t m_samples = 0;
		
		bool m_depthTest  = true;
		bool m_depthWrite = true;
		bool m_cull       = true;

		Settings::ECullFace m_cullFace = Settings::BACK;

		Resources::Texture* m_emptyTexture;
	};
}
