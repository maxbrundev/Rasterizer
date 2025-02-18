#pragma once

#include "Rendering/AShader.h"

namespace Rendering
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