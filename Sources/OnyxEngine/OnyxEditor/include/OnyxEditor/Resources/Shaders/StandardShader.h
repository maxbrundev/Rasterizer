#pragma once

#include <AmberGL/SoftwareRenderer/Programs/AProgram.h>

namespace OnyxEditor::Resources::Shaders
{
	class StandardShader : public AmberGL::SoftwareRenderer::Programs::AProgram
	{
	public:
		StandardShader();
		~StandardShader() override = default;

		std::unique_ptr<AProgram> clone() const override;

	protected:
		glm::vec4 VertexPass(const AmberGL::Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;
	};
}