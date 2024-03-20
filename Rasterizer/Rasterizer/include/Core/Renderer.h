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
		void Render() const;
		void Clear() const;

		SDL_Renderer* GetSDLRenderer() const;

	private:
		Context::Driver& m_driver;

		std::unique_ptr<Rasterizer> m_rasterizer;
	};
}
