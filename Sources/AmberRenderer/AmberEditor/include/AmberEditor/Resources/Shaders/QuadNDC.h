#pragma once

#include <AmberGL/SoftwareRenderer/Programs/AProgram.h>

namespace AmberEditor::Resources::Shaders
{
	class QuadNDC : public AmberGL::SoftwareRenderer::Programs::AProgram
	{
	public:
		QuadNDC() = default;
		~QuadNDC() override = default;

	protected:
		glm::vec4 VertexPass(const AmberGL::Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;
	};
}