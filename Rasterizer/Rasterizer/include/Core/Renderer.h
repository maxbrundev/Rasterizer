#pragma once

#include "Core/Rasterizer.h"
#include "Context/Driver.h"
#include "Rendering/AShader.h"
#include "Resources/Model.h"

namespace Rendering
{
	class Renderer
	{
	public:
		Renderer(Context::Driver& p_driver, Context::Window& p_window);
		~Renderer() = default;
		void Clear(const Data::Color& p_color);
		void ClearDepth();
		void Draw(Resources::Model& p_model, AShader& p_shader) const;
		void DrawMesh(Resources::Mesh& p_mesh, AShader& p_shader) const;
		void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, AShader& p_shader, const Data::Color& p_color);
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
