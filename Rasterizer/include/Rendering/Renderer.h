#pragma once

#include <SDL2/SDL_render.h>

#include "Driver.h"
#include "Context/SDLDriver.h"

#include "Rendering/Rasterizer.h"

#include "Rendering/AShader.h"
#include "Resources/Model.h"
#include "Resources/Texture.h"
#include "Settings/EPrimitiveMode.h"

#define OLD_RASTERIZER

namespace Rendering
{
	class Renderer
	{
	public:
		Renderer(Context::SDLDriver& p_SDLdriver, Driver& p_driver, Context::Window& p_window);
		~Renderer();

		void Clear(const Data::Color& p_color) const;
		void ClearDepth() const;
		void Draw(Resources::Model& p_model, Resources::Material* p_defaultMaterial);
		void DrawMesh(Settings::EPrimitiveMode p_drawMode, Resources::Mesh& p_mesh, const Resources::Material& p_material);
		void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, AShader& p_shader, const Data::Color& p_color);
		void Render() const;
		void Clear() const;
		void SetSamples(uint8_t p_samples) const;
		//TODO: FRAMEBUFFER BIND.
		void SendDataToGPU() const;
		uint8_t FetchState();
		void ApplyStateMask(uint8_t p_mask);

		SDL_Renderer* GetSDLRenderer() const;

#ifdef OLD_RASTERIZER
		void SetState(uint8_t p_state)
		{
			m_state = p_state;
		}

		void SetDepthTest(bool p_depthTest)
		{
			m_depthTest = p_depthTest;
		}

		void SetDepthWrite(bool p_depthWrite)
		{
			m_depthWrite = p_depthWrite;
		}

		void SetCull(bool p_value)
		{
			m_cull = p_value;
		}

		void SetCullFace(Settings::ECullFace p_cullFace)
		{
			m_cullFace = p_cullFace;
		}

		void SetRasterizationMode(Settings::ERasterizationMode p_rasterizationMode)
		{
			m_rasterizer->SetRasterizationMode(p_rasterizationMode);
		}
#endif

	private:
		void OnResize(uint16_t p_width, uint16_t p_height);

	private:
		Context::SDLDriver& m_driver;
		Driver& m_renderDriver;

		uint8_t m_state = 0;

		Resources::Texture* m_emptyTexture;

#ifdef OLD_RASTERIZER
		std::unique_ptr<Rasterizer> m_rasterizer;
		bool m_depthTest = true;
		bool m_depthWrite = true;
		bool m_cull = true;
		Settings::ECullFace m_cullFace = Settings::BACK;
#endif

		//TODO: FRAMEBUFFER CLASS
		SDL_Texture* m_sdlTexture;
	};
}
