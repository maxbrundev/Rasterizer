#include "Core/Renderer.h"

Core::Renderer::Renderer(Context::Driver& p_driver, Context::Window& p_window) : m_driver(p_driver)
{
	m_rasterizer = std::make_unique<Rasterizer>(p_window, m_driver.GetRenderer());
}

void Core::Renderer::Clear(const Data::Color& p_color)
{
	m_rasterizer->Clear(p_color);
}

void Core::Renderer::ClearDepth()
{
	m_rasterizer->ClearDepth();
}

void Core::Renderer::Draw(const Resources::Mesh& p_mesh, const glm::mat4& p_mvp, const glm::mat4& p_model) const
{
	m_rasterizer->RasterizeMesh(p_mesh, p_mvp, p_model);
}

void Core::Renderer::DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, const glm::mat4& p_mvp, const Data::Color& p_color)
{
	m_rasterizer->RasterizeLine({ p_point0 }, { p_point1 }, p_mvp, p_color);
}

void Core::Renderer::Render() const
{
	m_rasterizer->SendDataToGPU();

	m_driver.RenderCopy(m_rasterizer->GetTextureBuffer().GetSDLTexture());

	m_driver.RenderPresent();
}

void Core::Renderer::Clear() const
{
	m_driver.RenderClear();
}

SDL_Renderer* Core::Renderer::GetSDLRenderer() const
{
	return m_driver.GetRenderer();
}
