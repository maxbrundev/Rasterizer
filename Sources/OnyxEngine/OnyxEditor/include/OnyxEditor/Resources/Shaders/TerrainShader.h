#pragma once

#include <array>

#include <glm/glm.hpp>

#include "AmberGL/SoftwareRenderer/Programs/AProgram.h"

namespace OnyxEditor::Resources::Shaders
{
	class TerrainShader : public AmberGL::SoftwareRenderer::Programs::AProgram
	{
	public:
		TerrainShader();
		virtual ~TerrainShader() override = default;

	protected:
		glm::vec4 VertexPass(const AmberGL::Geometry::Vertex& p_vertex) override;
		glm::vec4 FragmentPass() override;

	public:
		std::unique_ptr<AProgram> clone() const override;

	private:
		const float atmosphereDensity = 0.0075f;

		std::array<glm::vec3, 5> m_colorLayers = 
		{
			glm::vec3(0.2f, 0.6f, 0.98f), // water
			glm::vec3(235.0f / 255.0f, 205.0f / 255.0f, 94.0f / 255.0f), // sand
			glm::vec3(0.2f, 0.6f, 0.1f), // grass
			glm::vec3(0.5f, 0.35f, 0.3f), // mountain
			glm::vec3(0.93f, 0.93f, 0.91f) // snow
		};

		std::array<float, 4> m_heights = { 0.0f, 0.6f, 2.5f, 12.0f };

		glm::vec3 m_lightDirection;
		glm::vec3 m_skyColor;
	};
}
