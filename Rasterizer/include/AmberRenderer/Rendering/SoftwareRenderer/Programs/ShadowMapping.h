#pragma once

#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/AProgram.h"

namespace AmberRenderer::Rendering::SoftwareRenderer::Programs
{
	class ShadowMapping : public AProgram
	{
	public:
		ShadowMapping() = default;
		~ShadowMapping() override = default;

	protected:
		glm::vec4 VertexPass(const Geometry::Vertex& p_vertex) override;
		Data::Color FragmentPass() override;
	};
}