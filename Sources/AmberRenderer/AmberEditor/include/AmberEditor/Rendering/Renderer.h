#pragma once

#include <SDL2/SDL_render.h>

#include <AmberGL/SoftwareRenderer/Programs/AProgram.h>

#include "AmberEditor/Context/IDisplay.h"
#include "AmberEditor/Context/SDLDriver.h"

#include "AmberEditor/Data/Color.h"

#include "AmberEditor/Rendering/Driver.h"
#include "AmberEditor/Rendering/Settings/EPrimitiveMode.h"

#include "AmberEditor/Resources/Model.h"
#include "AmberEditor/Resources/Texture.h"
#include "AmberEditor/Resources/Material.h"

namespace AmberEditor::Rendering
{
	class Renderer
	{
	public:
		Renderer(Driver& p_driver, Context::IDisplay& p_display);
		~Renderer();

		void SetClearColor(const glm::vec4& p_color);
		void Clear(bool p_colorBuffer, bool p_depthBuffer) const;
		void SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const;
		void Draw(Resources::Model& p_model, Resources::Material* p_defaultMaterial);
		void DrawMesh(Settings::EPrimitiveMode p_drawMode, Resources::Mesh& p_mesh, const Resources::Material& p_material);
		void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, AmberGL::SoftwareRenderer::Programs::AProgram& p_shader, const AmberEditor::Data::Color& p_color);
		void SetSamples(uint8_t p_samples) const;
		
		void SendDataToGPU() const;
		uint8_t FetchState() const;
		void ApplyStateMask(uint8_t p_mask);

		void DisplayPresent() const;
		void DisplayClear() const;

	private:
		void OnResize(uint16_t p_width, uint16_t p_height);

	private:
		Driver& m_driver;
		Context::IDisplay& m_display;

		Resources::Texture* m_emptyTexture;

		uint8_t m_state = 0;
	};
}
