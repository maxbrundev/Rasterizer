#pragma once

#include <SDL2/SDL_render.h>

#include "Driver.h"
#include "Context/SDLDriver.h"

#include "Rendering/Rasterizer.h"

#include "Rendering/Settings/ERasterizationMode.h"

#include "Rendering/AShader.h"
#include "Resources/Model.h"
#include "Resources/Texture.h"
#include "Settings/EPrimitiveMode.h"

namespace Rendering
{
	class Renderer
	{
	public:
		Renderer(Context::SDLDriver& p_SDLdriver, Rendering::Driver& p_driver, Context::Window& p_window);
		~Renderer();

		void Clear(const Data::Color& p_color) const;
		void ClearDepth() const;
		void Draw(Resources::Model& p_model, Resources::Material* p_defaultMaterial);
		void DrawMesh(Settings::EPrimitiveMode p_drawMode, Resources::Mesh& p_mesh, const Resources::Material& p_material);
		void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, AShader& p_shader, const Data::Color& p_color);
		void Render() const;
		void Clear() const;
		void SetSamples(uint8_t p_samples) const;
		//TODO FRAMEBUFFFER BIND
		void SendDataToGPU() const;
		uint8_t FetchState();
		void ApplyStateMask(uint8_t p_mask);

		SDL_Renderer* GetSDLRenderer() const;

	private:

		void OnResize(uint16_t p_width, uint16_t p_height);

		Context::SDLDriver& m_driver;
		Driver& m_renderDriver;

		uint8_t m_state = 0;
		//std::unique_ptr<Rasterizer> m_rasterizer;

		Resources::Texture* m_emptyTexture;

		SDL_Texture* m_sdlTexture;
	};
}
