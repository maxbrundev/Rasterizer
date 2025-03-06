#pragma once

#include "AmberRenderer/Rendering/Rasterizer/Shaders/AShader.h"

namespace AmberRenderer::Rendering::Rasterizer::Shaders
{
	class ShadowMapping : public AShader
	{
	public:
		ShadowMapping() = default;
		~ShadowMapping() override = default;

	protected:
		glm::vec4 VertexPass(const Geometry::Vertex& p_vertex) override;
		Data::Color FragmentPass() override;
	};
}