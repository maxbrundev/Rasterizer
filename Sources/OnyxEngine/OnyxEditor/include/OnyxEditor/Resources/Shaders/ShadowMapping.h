#pragma once

#include <AmberGL/SoftwareRenderer/Programs/AProgram.h>

namespace OnyxEditor::Resources::Shaders
{
	class ShadowMapping : public AmberGL::SoftwareRenderer::Programs::AProgram
	{
	public:
		ShadowMapping();
		~ShadowMapping() override = default;

		std::unique_ptr<AProgram> clone() const override;

	protected:
		glm::vec4 VertexPass(const AmberGL::Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;

		float CalculateShadow(const glm::vec4& p_fragPosLightSpace, const glm::vec3& p_geometricNormal, const glm::vec3& p_lightDir, float p_shadowMapSize, int p_pcfRadius, float p_minBias, float p_maxBias) const;
	};
}
