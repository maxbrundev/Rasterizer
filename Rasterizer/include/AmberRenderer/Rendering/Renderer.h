#pragma once

#include <SDL2/SDL_render.h>

#include "AmberRenderer/Rendering/Driver.h"
#include "AmberRenderer/Context/SDLDriver.h"

#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/AProgram.h"
#include "AmberRenderer/Resources/Model.h"
#include "AmberRenderer/Resources/Texture.h"
#include "AmberRenderer/Rendering/Settings/EPrimitiveMode.h"

namespace AmberRenderer::Rendering
{
	class Renderer
	{
	public:
		Renderer(Context::SDLDriver& p_SDLDriver, Driver& p_driver);
		~Renderer();

		void SetClearColor(const glm::vec4& p_color);
		void Clear(bool p_colorBuffer, bool p_depthBuffer) const;
		void SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const;
		void Draw(Resources::Model& p_model, Resources::Material* p_defaultMaterial);
		void DrawMesh(Settings::EPrimitiveMode p_drawMode, Resources::Mesh& p_mesh, const Resources::Material& p_material);
		void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, SoftwareRenderer::Programs::AProgram& p_shader, const Data::Color& p_color);
		void Render() const;
		void RenderClear() const;
		void SetSamples(uint8_t p_samples) const;
		//TODO: FRAMEBUFFER BIND.
		void SendDataToGPU() const;
		uint8_t FetchState() const;
		void ApplyStateMask(uint8_t p_mask);

		SDL_Renderer* GetSDLRenderer() const;

	private:
		void OnResize(uint16_t p_width, uint16_t p_height);

	private:
		Context::SDLDriver& m_SDLDriver;
		Driver& m_driver;

		uint8_t m_state = 0;

		Resources::Texture* m_emptyTexture;
		//TODO:
		//Resources::Material m_defaultMaterial;
		SDL_Texture* m_sdlTexture;
	};
}
