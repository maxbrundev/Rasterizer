#pragma once

#include "AmberGL/API/Export.h"

#include "AmberGL/SoftwareRenderer/Programs/AProgram.h"

namespace AmberGL::SoftwareRenderer::Programs
{
	class API_AMBERGL QuadNDC : public AProgram
	{
	public:
		QuadNDC() = default;
		~QuadNDC() override = default;

	protected:
		glm::vec4 VertexPass(const Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;
	};
}