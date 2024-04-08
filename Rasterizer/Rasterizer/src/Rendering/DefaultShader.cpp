#include "Rendering/DefaultShader.h"

glm::vec4 Rendering::DefaultShader::VertexPass(const Geometry::Vertex& p_vertex)
{
	const glm::mat4 mvp = GetUniform<glm::mat4>("mvp");
	const glm::mat4 modelMatrix = GetUniform<glm::mat4>("u_Model");

	const glm::vec4 vertexWorldPosition = mvp * glm::vec4(p_vertex.position, 1.0f);

	glm::vec3 vertexNormal = glm::mat3(transpose(inverse(modelMatrix))) * p_vertex.normal;

	vertexNormal = glm::normalize(vertexNormal);

	SetVarying("v_TextCoords", p_vertex.textCoords);
	SetVarying("v_Normal", vertexNormal);

	return vertexWorldPosition;
}

Data::Color Rendering::DefaultShader::FragmentPass()
{
	glm::vec2 textCoords = GetVarying<glm::vec2>("v_TextCoords");
	auto texture = GetSample<Resources::Texture>("u_DiffuseMap");

	glm::vec4 diffuse = Texture(*texture, textCoords);

	return diffuse;
}
