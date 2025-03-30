#pragma once

#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/AProgram.h"

namespace AmberRenderer::Rendering::SoftwareRenderer::Programs
{
	class ShadowMappingDepth : public AProgram
	{
	public:
		ShadowMappingDepth() = default;
		~ShadowMappingDepth() override = default;

	protected:
		glm::vec4 VertexPass(const Geometry::Vertex& p_vertex) override;
		Data::Color FragmentPass() override;
	};
}