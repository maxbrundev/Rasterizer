#include "Rendering/Renderer.h"

#include "Rendering/GLRasterizer.h"
#include "Rendering/Settings/ERenderState.h"
#include "Resources/Loaders/TextureLoader.h"

Rendering::Renderer::Renderer(Context::Driver& p_driver, Context::Window& p_window) :
m_driver(p_driver),
m_emptyTexture(Resources::Loaders::TextureLoader::CreateColor
(
	(255 << 24) | (255 << 16) | (255 << 8) | 255,
	Resources::Settings::ETextureFilteringMode::NEAREST,
	Resources::Settings::ETextureWrapMode::CLAMP
))
{
	GLRasterizer::MakeCurrentContext(&p_window, m_driver.GetRenderer(), 800, 600);
	//m_rasterizer = std::make_unique<Rasterizer>(p_window, m_driver.GetRenderer(), 800, 600);
}

Rendering::Renderer::~Renderer()
{
	Resources::Loaders::TextureLoader::Destroy(m_emptyTexture);
}

void Rendering::Renderer::Clear(const Data::Color& p_color) const
{
	GLRasterizer::Clear(p_color);
	//m_rasterizer->Clear(p_color);
}

void Rendering::Renderer::ClearDepth() const
{
	GLRasterizer::ClearDepth();
	//m_rasterizer->ClearDepth();
}

void Rendering::Renderer::Draw(Settings::EDrawMode p_drawMode, Resources::Model& p_model, Resources::Material* p_defaultMaterial)
{
	uint8_t state = FetchState();

	ApplyStateMask(state);


	//m_rasterizer->SetState(state);

	auto materials = p_model.GetMaterials();

	for (const auto mesh : p_model.GetMeshes())
	{
		if (mesh->GetMaterialIndex() < 255)
		{
			const Resources::Material* material = materials.at(mesh->GetMaterialIndex());

			if (!material || !material->GetShader())
				material = p_defaultMaterial;

			DrawMesh(p_drawMode, *mesh, *material);
		}
	}
}

void Rendering::Renderer::DrawMesh(Settings::EDrawMode p_drawMode, Resources::Mesh& p_mesh, const Resources::Material& p_material)
{
	if (p_material.GetShader() != nullptr)
	{
		uint8_t state = FetchState();

		ApplyStateMask(state);

		//m_rasterizer->SetState(state);

		p_material.Bind(m_emptyTexture);
		GLRasterizer::DrawElements(p_drawMode, p_mesh);

		//m_rasterizer->RasterizeMesh(p_drawMode, p_mesh, *p_material.GetShader());
	}
}

void Rendering::Renderer::DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, AShader& p_shader, const Data::Color& p_color)
{
	uint8_t state = FetchState();

	ApplyStateMask(state);

	GLRasterizer::SetState(state);
	//m_rasterizer->SetState(state);
	p_shader.Bind();
	GLRasterizer::DrawLine({ p_point0 }, { p_point1 }, p_color);
	//m_rasterizer->RasterizeLine({ p_point0 }, { p_point1 }, p_shader, p_color);
}

void Rendering::Renderer::Render() const
{
	//m_rasterizer->SendDataToGPU();
	GLRasterizer::SendDataToGPU();
	m_driver.RenderCopy(GLRasterizer::GetTextureBuffer().GetSDLTexture());

	m_driver.RenderPresent();
}

void Rendering::Renderer::Clear() const
{
	m_driver.RenderClear();
}

void Rendering::Renderer::SetSamples(uint8_t p_samples) const
{
	GLRasterizer::SetSamples(p_samples);
	//m_rasterizer->SetSamples(p_samples);
}

uint8_t Rendering::Renderer::FetchState() const
{
	uint8_t result = 0;
	
	if (GLRasterizer::GetBool(GLR_DEPTH_WRITE))
		result |= GLR_DEPTH_WRITE;
	if (GLRasterizer::GetCapability(GLR_CULL_FACE))
		result |= GLR_CULL_FACE;
	if (GLRasterizer::GetCapability(GLR_DEPTH_TEST))
		result |= GLR_DEPTH_TEST;

	int face = GLRasterizer::GetInt(GLR_CULL_FACE);
	switch (face)
	{
	case GLR_BACK:
		result |= GLR_BACK;
		break;
	case GLR_FRONT:
		result |= GLR_FRONT;
		break;
	default:
		break;
	}
	return result;
}

void Rendering::Renderer::ApplyStateMask(uint8_t p_mask)
{
	if (p_mask != m_state)
	{
		if ((p_mask & GLR_DEPTH_WRITE) != (m_state & GLR_DEPTH_WRITE))
			GLRasterizer::DepthMask((p_mask & GLR_DEPTH_WRITE) != 0);

		if ((p_mask & GLR_DEPTH_TEST) != (m_state & GLR_DEPTH_TEST))
		{
			if (p_mask & GLR_DEPTH_TEST)
				GLRasterizer::Enable(GLR_DEPTH_TEST);
			else
				GLRasterizer::Disable(GLR_DEPTH_TEST);
		}

		if ((p_mask & GLR_CULL_FACE) != (m_state & GLR_CULL_FACE))
		{
			if (p_mask & GLR_CULL_FACE)
				GLRasterizer::Enable(GLR_CULL_FACE);
			else
				GLRasterizer::Disable(GLR_CULL_FACE);
		}

		if ((p_mask & (GLR_BACK | GLR_FRONT)) != (m_state & (GLR_BACK | GLR_FRONT)))
		{
			int backBit = p_mask & GLR_BACK;
			int frontBit = p_mask & GLR_FRONT;
			GLRasterizer::CullFace((backBit && frontBit) ? (GLR_BACK | GLR_FRONT) : (backBit ? GLR_BACK : GLR_FRONT));
		}

		m_state = p_mask;
	}
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
