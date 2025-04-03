#include "AmberEditor/Rendering/Renderer.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>
#include "AmberEditor/Rendering/Settings/EPrimitiveMode.h"
#include "AmberEditor/Resources/Loaders/TextureLoader.h"

AmberEditor::Rendering::Renderer::Renderer(Context::SDLDriver& p_SDLDriver, Driver& p_driver) :
m_SDLDriver(p_SDLDriver),
m_driver(p_driver),
m_emptyTexture(Resources::Loaders::TextureLoader::CreateColor
(
	(255 << 24) | (255 << 16) | (255 << 8) | 255,
	Resources::Settings::ETextureFilteringMode::NEAREST,
	Resources::Settings::ETextureFilteringMode::NEAREST
)),
m_sdlTexture(nullptr)
{
	m_sdlTexture = SDL_CreateTexture(m_SDLDriver.GetRenderer(), SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, static_cast<int>(800), static_cast<int>(600));

	//p_window.ResizeEvent.AddListener(std::bind(&Renderer::OnResize, this, std::placeholders::_1, std::placeholders::_2));
}

AmberEditor::Rendering::Renderer::~Renderer()
{
	Resources::Loaders::TextureLoader::Destroy(m_emptyTexture);

	SDL_DestroyTexture(m_sdlTexture);
}

void AmberEditor::Rendering::Renderer::SetClearColor(const glm::vec4& p_color)
{
	m_driver.SetClearColor(p_color);
}

void AmberEditor::Rendering::Renderer::Clear(bool p_colorBuffer, bool p_depthBuffer) const
{
	m_driver.Clear(p_colorBuffer, p_depthBuffer);
}

void AmberEditor::Rendering::Renderer::SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const
{
	m_driver.SetViewport(p_x, p_y, p_width, p_height);
}

void AmberEditor::Rendering::Renderer::Draw(Resources::Model& p_model, Resources::Material* p_defaultMaterial)
{
	uint8_t state = FetchState();

	ApplyStateMask(state);

	auto& materials = p_model.GetMaterials();

	for (const auto mesh : p_model.GetMeshes())
	{
		if (mesh->GetMaterialIndex() < 255)
		{
			const Resources::Material* material = materials.at(mesh->GetMaterialIndex());

			if (!material || !material->GetShader())
				material = p_defaultMaterial;

			DrawMesh(Settings::EPrimitiveMode::TRIANGLES, *mesh, *material);
		}
	}
}

void AmberEditor::Rendering::Renderer::DrawMesh(Settings::EPrimitiveMode p_drawMode, Resources::Mesh& p_mesh, const Resources::Material& p_material)
{
	if (p_material.GetShader() != nullptr)
	{
		uint8_t state = FetchState();

		ApplyStateMask(state);

		p_material.Bind(m_emptyTexture);

		p_mesh.Bind();

		if (p_mesh.GetIndexCount() > 0)
		{
			AmberGL::DrawElements(static_cast<uint8_t>(p_drawMode), p_mesh.GetIndexCount());
		}
		else
		{
			AmberGL::DrawArrays(static_cast<uint8_t>(p_drawMode), 0, p_mesh.GetVertexCount());
		}
		p_mesh.Unbind();

		p_material.Unbind();
	}
}

void AmberEditor::Rendering::Renderer::DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, AmberGL::SoftwareRenderer::Programs::AProgram& p_shader, const AmberGL::Data::Color& p_color)
{
	uint8_t state = FetchState();

	ApplyStateMask(state);

	p_shader.Bind();

	AmberGL::DrawLine({ p_point0 }, { p_point1 }, p_color);
}

void AmberEditor::Rendering::Renderer::Render() const
{
	SendDataToGPU();

	m_SDLDriver.RenderCopy(m_sdlTexture);

	m_SDLDriver.RenderPresent();
}

void AmberEditor::Rendering::Renderer::RenderClear() const
{
	m_SDLDriver.RenderClear();
}

void AmberEditor::Rendering::Renderer::SetSamples(uint8_t p_samples) const
{
	AmberGL::SetSamples(p_samples);
}

void AmberEditor::Rendering::Renderer::SendDataToGPU() const
{
	SDL_UpdateTexture(m_sdlTexture, nullptr, AmberGL::GetFrameBufferData(), AmberGL::GetFrameBufferRowSize());
}

uint8_t AmberEditor::Rendering::Renderer::FetchState() const
{
	uint8_t result = 0;

	if (m_driver.GetBool(AGL_DEPTH_WRITE))                                  result |= 0b0000'0001;
	if (m_driver.GetCapability(Settings::ERenderingCapability::DEPTH_TEST)) result |= 0b0000'0010;
	if (m_driver.GetCapability(Settings::ERenderingCapability::CULL_FACE))  result |= 0b0000'0100;

	switch (static_cast<Settings::ECullFace>(m_driver.GetInt(AGL_CULL_FACE)))
	{
	case Settings::ECullFace::BACK:           result |= 0b0000'1000; break;
	case Settings::ECullFace::FRONT:          result |= 0b0001'0000; break;
	case Settings::ECullFace::FRONT_AND_BACK: result |= 0b0010'0000; break;
	}

	return result;
}

void AmberEditor::Rendering::Renderer::ApplyStateMask(uint8_t p_mask)
{
	if (p_mask != m_state)
	{

		if ((p_mask & 0x01) != (m_state & 0x01)) m_driver.SetDepthWriting(p_mask & 0x01);
		if ((p_mask & 0x02) != (m_state & 0x02)) m_driver.SetCapability(Settings::ERenderingCapability::DEPTH_TEST, p_mask & 0x02);
		if ((p_mask & 0x04) != (m_state & 0x04)) m_driver.SetCapability(Settings::ERenderingCapability::CULL_FACE, p_mask & 0x04);

		if ((p_mask & 0x04) && ((p_mask & 0x08) != (m_state & 0x08) || (p_mask & 0x10) != (m_state & 0x10)))
		{
			const int backBit = p_mask & 0x08;
			const int frontBit = p_mask & 0x10;
			m_driver.SetCullFace(backBit && frontBit ? Settings::ECullFace::FRONT_AND_BACK : (backBit ? Settings::ECullFace::BACK : Settings::ECullFace::FRONT));
		}

		m_state = p_mask;
	}
}

SDL_Renderer* AmberEditor::Rendering::Renderer::GetSDLRenderer() const
{
	return m_SDLDriver.GetRenderer();
}

void AmberEditor::Rendering::Renderer::OnResize(uint16_t p_width, uint16_t p_height)
{
	//m_textureBuffer.Resize(p_width, p_height);
	//m_depthBuffer.Resize(p_width, p_height);

	//SDL_DestroyTexture(m_sdlTexture);

	//m_sdlTexture = SDL_CreateTexture(m_driver.GetRenderer(), SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, static_cast<int>(GLRasterizer::GetTextureBuffer().GetWidth()), static_cast<int>(GLRasterizer::GetTextureBuffer().GetHeight()));
}
