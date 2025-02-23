#include "Rendering/Renderer.h"


#include "Rendering/Rasterizer/GLRasterizer.h"
#include "Rendering/Settings/EPrimitiveMode.h"
#include "Resources/Loaders/TextureLoader.h"

Rendering::Renderer::Renderer(Context::SDLDriver& p_SDLdriver, Driver& p_driver, Context::Window& p_window) :
m_driver(p_SDLdriver),
m_renderDriver(p_driver),
m_emptyTexture(Resources::Loaders::TextureLoader::CreateColor
(
	(255 << 24) | (255 << 16) | (255 << 8) | 255,
	Resources::Settings::ETextureFilteringMode::NEAREST,
	Resources::Settings::ETextureWrapMode::CLAMP
)),
m_sdlTexture(nullptr)
{
	GLRasterizer::Initialize(800, 600);

	m_sdlTexture = SDL_CreateTexture(m_driver.GetRenderer(), SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, static_cast<int>(800), static_cast<int>(600));

	p_window.ResizeEvent.AddListener(std::bind(&Renderer::OnResize, this, std::placeholders::_1, std::placeholders::_2));
}

Rendering::Renderer::~Renderer()
{
	Resources::Loaders::TextureLoader::Destroy(m_emptyTexture);

	SDL_DestroyTexture(m_sdlTexture);
}

void Rendering::Renderer::Clear(const Data::Color& p_color) const
{
	GLRasterizer::Clear(p_color);
}

void Rendering::Renderer::ClearDepth() const
{
	GLRasterizer::ClearDepth();
}

void Rendering::Renderer::Draw(Resources::Model& p_model, Resources::Material* p_defaultMaterial)
{
	uint8_t state = FetchState();

	ApplyStateMask(state);

	auto materials = p_model.GetMaterials();

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

void Rendering::Renderer::DrawMesh(Settings::EPrimitiveMode p_drawMode, Resources::Mesh& p_mesh, const Resources::Material& p_material)
{
	if (p_material.GetShader() != nullptr)
	{
		uint8_t state = FetchState();

		ApplyStateMask(state);

		p_material.Bind(m_emptyTexture);

		p_mesh.Bind();
		GLRasterizer::DrawElements(static_cast<uint8_t>(p_drawMode), p_mesh.GetIndexCount());
		p_mesh.Unbind();
	}
}

void Rendering::Renderer::DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, Rasterizer::Shaders::AShader& p_shader, const Data::Color& p_color)
{
	uint8_t state = FetchState();

	ApplyStateMask(state);

	p_shader.Bind();

	GLRasterizer::DrawLine({ p_point0 }, { p_point1 }, p_color);
}

void Rendering::Renderer::Render() const
{
	SendDataToGPU();

	m_driver.RenderCopy(m_sdlTexture);

	m_driver.RenderPresent();
}

void Rendering::Renderer::Clear() const
{
	m_driver.RenderClear();
}

void Rendering::Renderer::SetSamples(uint8_t p_samples) const
{
	GLRasterizer::SetSamples(p_samples);
}

void Rendering::Renderer::SendDataToGPU() const
{
	SDL_UpdateTexture(m_sdlTexture, nullptr, GLRasterizer::GetTextureBuffer().GetData(), GLRasterizer::GetTextureBuffer().GetRawSize());
}

uint8_t Rendering::Renderer::FetchState()
{
	uint8_t result = 0;

	if (m_renderDriver.GetBool(GLR_DEPTH_WRITE))             result |= 0b0000'0001;
	if (m_renderDriver.GetCapability(Settings::DEPTH_TEST))  result |= 0b0000'0010;
	if (m_renderDriver.GetCapability(Settings::CULL_FACE))   result |= 0b0000'0100;

	switch (static_cast<Settings::ECullFace>(m_renderDriver.GetInt(GLR_CULL_FACE)))
	{
	case Settings::ECullFace::BACK:           result |= 0b0000'1000; break;
	case Settings::ECullFace::FRONT:          result |= 0b0001'0000; break;
	case Settings::ECullFace::FRONT_AND_BACK: result |= 0b0010'0000; break;
	}

	return result;
}

void Rendering::Renderer::ApplyStateMask(uint8_t p_mask)
{
	if (p_mask != m_state)
	{

		if ((p_mask & 0x01) != (m_state & 0x01)) m_renderDriver.SetDepthWriting(p_mask & 0x01);
		if ((p_mask & 0x02) != (m_state & 0x02)) m_renderDriver.SetCapability(Settings::DEPTH_TEST, p_mask & 0x02);
		if ((p_mask & 0x04) != (m_state & 0x04)) m_renderDriver.SetCapability(Settings::CULL_FACE, p_mask & 0x04);

		if ((p_mask & 0x04) && ((p_mask & 0x08) != (m_state & 0x08) || (p_mask & 0x10) != (m_state & 0x10)))
		{
			const int backBit = p_mask & 0x08;
			const int frontBit = p_mask & 0x10;
			m_renderDriver.SetCullFace(backBit && frontBit ? Settings::FRONT_AND_BACK : (backBit ? Settings::BACK : Settings::FRONT));
		}

		m_state = p_mask;
	}
}

SDL_Renderer* Rendering::Renderer::GetSDLRenderer() const
{
	return m_driver.GetRenderer();
}

void Rendering::Renderer::OnResize(uint16_t p_width, uint16_t p_height)
{
	//m_textureBuffer.Resize(p_width, p_height);
	//m_depthBuffer.Resize(p_width, p_height);

	//SDL_DestroyTexture(m_sdlTexture);

	//m_sdlTexture = SDL_CreateTexture(m_driver.GetRenderer(), SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, static_cast<int>(GLRasterizer::GetTextureBuffer().GetWidth()), static_cast<int>(GLRasterizer::GetTextureBuffer().GetHeight()));
}
