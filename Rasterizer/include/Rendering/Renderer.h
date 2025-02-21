#pragma once

#include <SDL2/SDL_render.h>

#include "Context/Driver.h"

#include "Rendering/Rasterizer.h"

#include "Rendering/Settings/EDrawMode.h"

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
		//TODO FRAMEBUFFFER BIND
		void SendDataToGPU() const;
		uint8_t FetchState() const;
		void ApplyStateMask(uint8_t p_mask);

		SDL_Renderer* GetSDLRenderer() const;

	private:

		void OnResize(uint16_t p_width, uint16_t p_height);

		Context::Driver& m_driver;

		uint8_t m_state;
		//std::unique_ptr<Rasterizer> m_rasterizer;

		Resources::Texture* m_emptyTexture;

		SDL_Texture* m_sdlTexture;
	};
}
