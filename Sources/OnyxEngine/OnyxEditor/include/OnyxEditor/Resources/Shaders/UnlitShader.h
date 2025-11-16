#pragma once

#include <AmberGL/SoftwareRenderer/Programs/AProgram.h>

namespace OnyxEditor::Resources::Shaders
{
	class UnlitShader : public AmberGL::SoftwareRenderer::Programs::AProgram
	{
	public:
		UnlitShader();
		~UnlitShader() override = default;

	protected:
		glm::vec4 VertexPass(const AmberGL::Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;

	public:
		std::unique_ptr<AProgram> clone() const override;
	};
}