#pragma once

#include "AmberGL/API/Export.h"

#include "AmberGL/SoftwareRenderer/Programs/AProgram.h"

namespace AmberGL::SoftwareRenderer::Programs
{
	class API_AMBERGL StandardShader : public AProgram
	{
	public:
		StandardShader() = default;
		~StandardShader() override = default;

	protected:
		glm::vec4 VertexPass(const Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;
	};
}