#pragma once

#include "Core/Rasterizer.h"
#include "Resources/Mesh.h"
#include "Context/Driver.h"

namespace Core
{
	class Renderer
	{
	public:
		Renderer(Context::Driver& p_driver, Context::Window& p_window);
		~Renderer() = default;
		void Clear(const Data::Color& p_color);
		void ClearDepth();
		void Draw(const Resources::Mesh& p_mesh, const glm::mat4& p_mvp, const glm::mat4& p_model) const;
		void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, const glm::mat4& p_mvp, const Data::Color& p_color);
		void Render() const;
		void Clear() const;

		void SetCullFace(ECullFace p_cullFace) const;
		void SetDepthTest(bool p_depthTest) const;
		void SetDepthWrite(bool p_depthWrite) const;

		SDL_Renderer* GetSDLRenderer() const;

	private:
		Context::Driver& m_driver;
		std::unique_ptr<Rasterizer> m_rasterizer;
	};
}
