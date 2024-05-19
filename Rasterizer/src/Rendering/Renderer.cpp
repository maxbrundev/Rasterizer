#include "Rendering/Renderer.h"

#include "Rendering/Settings/ERenderState.h"

Rendering::Renderer::Renderer(Context::Driver& p_driver, Context::Window& p_window) : m_driver(p_driver)
{
	m_rasterizer = std::make_unique<Rasterizer>(p_window, m_driver.GetRenderer(), 800, 600);
}

void Rendering::Renderer::Clear(const Data::Color& p_color) const
{
	m_rasterizer->Clear(p_color);
}

void Rendering::Renderer::ClearDepth() const
{
	m_rasterizer->ClearDepth();
}

void Rendering::Renderer::Draw(Settings::EDrawMode p_drawMode, Resources::Model& p_model, AShader& p_shader)
{
	uint8_t state = FetchState();

	ApplyState(state);

	m_rasterizer->SetState(state);

	for (const auto mesh : p_model.GetMeshes())
	{
		DrawMesh(p_drawMode, *mesh, p_shader);
	}
}

void Rendering::Renderer::DrawMesh(Settings::EDrawMode p_drawMode, Resources::Mesh& p_mesh, AShader& p_shader)
{
	uint8_t state = FetchState();

	ApplyState(state);

	m_rasterizer->SetState(state);

	m_rasterizer->RasterizeMesh(p_drawMode, p_mesh, p_shader);
}

void Rendering::Renderer::DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, AShader& p_shader, const Data::Color& p_color)
{
	uint8_t state = FetchState();

	ApplyState(state);

	m_rasterizer->SetState(state);

	m_rasterizer->RasterizeLine({ p_point0 }, { p_point1 }, p_shader, p_color);
}

void Rendering::Renderer::Render() const
{
	m_rasterizer->SendDataToGPU();

	m_driver.RenderCopy(m_rasterizer->GetTextureBuffer().GetSDLTexture());

	m_driver.RenderPresent();
}

void Rendering::Renderer::Clear() const
{
	m_driver.RenderClear();
}

void Rendering::Renderer::SetSamples(uint8_t p_samples) const
{
	m_rasterizer->SetSamples(p_samples);
}

uint8_t Rendering::Renderer::FetchState() const
{
	uint8_t result = 0;

	if (m_depthWrite) result |= Settings::ERenderState::DEPTH_WRITE;
	if (m_depthTest)  result |= Settings::ERenderState::DEPTH_TEST;

	if(m_cull)
	{
		result |= Settings::ERenderState::CULL_FACE;

		switch (m_cullFace)
		{
		case Settings::ECullFace::BACK:
			result |= Settings::ECullFace::BACK;
			break;
		case Settings::ECullFace::FRONT:
			result |= Settings::ECullFace::FRONT;
			break;
		}
	}

	return result;
}

void Rendering::Renderer::ApplyState(uint8_t p_state)
{
	if(m_state == p_state)
		return;
	
	m_depthWrite = p_state & Settings::ERenderState::DEPTH_WRITE;
	m_depthTest  = p_state & Settings::ERenderState::DEPTH_TEST;

	if(p_state & Settings::ERenderState::CULL_FACE)
	{
		m_cull = true;

		if (p_state & Settings::ECullFace::BACK)
		{
			m_cullFace = Settings::ECullFace::BACK;
		}
		else if (p_state & Settings::ECullFace::FRONT)
		{
			m_cullFace = Settings::ECullFace::FRONT;
		}
	}

	m_state = p_state;
}

void Rendering::Renderer::SetState(uint8_t p_state)
{
	m_state = p_state;
}

void Rendering::Renderer::SetDepthTest(bool p_depthTest)
{
	m_depthTest = p_depthTest;
}

void Rendering::Renderer::SetDepthWrite(bool p_depthWrite)
{
	m_depthWrite = p_depthWrite;
}

void Rendering::Renderer::SetCull(bool p_value)
{
	m_cull = p_value;
}

void Rendering::Renderer::SetCullFace(Settings::ECullFace p_cullFace)
{
	m_cullFace = p_cullFace;
}

SDL_Renderer* Rendering::Renderer::GetSDLRenderer() const
{
	return m_driver.GetRenderer();
}
