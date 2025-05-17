#pragma once

#include <AmberGL/SoftwareRenderer/Programs/AProgram.h>

namespace AmberEditor::Resources::Shaders
{
	class ShadowMappingDepth : public AmberGL::SoftwareRenderer::Programs::AProgram
	{
	public:
		ShadowMappingDepth();
		~ShadowMappingDepth() override = default;

	protected:
		glm::vec4 VertexPass(const AmberGL::Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;
	};
}