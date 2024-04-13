#include "Rendering/Renderer.h"

Rendering::Renderer::Renderer(Context::Driver& p_driver, Context::Window& p_window) : m_driver(p_driver)
{
	m_rasterizer = std::make_unique<Rasterizer>(p_window, m_driver.GetRenderer(), 800, 600);
}

void Rendering::Renderer::Clear(const Data::Color& p_color)
{
	m_rasterizer->Clear(p_color);
}

void Rendering::Renderer::ClearDepth()
{
	m_rasterizer->ClearDepth();
}

void Rendering::Renderer::Draw(Resources::Model& p_model, AShader& p_shader) const
{
	for (const auto mesh : p_model.GetMeshes())
	{
		DrawMesh(*mesh, p_shader);
	}
}

void Rendering::Renderer::DrawMesh(Resources::Mesh& p_mesh, AShader& p_shader) const
{
	m_rasterizer->RasterizeMesh(p_mesh, p_shader);
}

void Rendering::Renderer::DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, AShader& p_shader, const Data::Color& p_color)
{
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

void Rendering::Renderer::SetCullFace(ECullFace p_cullFace) const
{
	m_rasterizer->GetRenderState().CullFace = p_cullFace;
}

void Rendering::Renderer::SetDepthTest(bool p_depthTest) const
{
	m_rasterizer->GetRenderState().DepthTest = p_depthTest;
}

void Rendering::Renderer::SetDepthWrite(bool p_depthWrite) const
{
	m_rasterizer->GetRenderState().DepthWrite = p_depthWrite;
}

SDL_Renderer* Rendering::Renderer::GetSDLRenderer() const
{
	return m_driver.GetRenderer();
}
