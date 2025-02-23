#pragma once

#include "Rendering/Rasterizer/Shaders/AShader.h"

namespace Rendering::Rasterizer::Shaders
{
	class DefaultShader : public AShader
	{
	public:
		DefaultShader() = default;
		~DefaultShader() override = default;

	protected:
		glm::vec4 VertexPass(const Geometry::Vertex& p_vertex) override;
		Data::Color FragmentPass() override;
	};
}