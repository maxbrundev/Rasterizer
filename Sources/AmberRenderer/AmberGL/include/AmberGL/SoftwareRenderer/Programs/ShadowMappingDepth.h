#pragma once

#include "AmberGL/API/Export.h"

#include "AmberGL/SoftwareRenderer/Programs/AProgram.h"

namespace AmberGL::SoftwareRenderer::Programs
{
	class API_AMBERGL ShadowMappingDepth : public AProgram
	{
	public:
		ShadowMappingDepth() = default;
		~ShadowMappingDepth() override = default;

	protected:
		glm::vec4 VertexPass(const Geometry::Vertex& p_vertex) override;
		Data::Color FragmentPass() override;
	};
}