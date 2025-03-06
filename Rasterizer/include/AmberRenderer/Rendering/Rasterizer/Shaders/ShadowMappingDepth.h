#pragma once

#include "AmberRenderer/Rendering/Rasterizer/Shaders/AShader.h"

namespace AmberRenderer::Rendering::Rasterizer::Shaders
{
	class ShadowMappingDepth : public AShader
	{
	public:
		ShadowMappingDepth() = default;
		~ShadowMappingDepth() override = default;

	protected:
		glm::vec4 VertexPass(const Geometry::Vertex& p_vertex) override;
		Data::Color FragmentPass() override;
	};
}