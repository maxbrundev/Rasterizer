#include "Rendering/DefaultShader.h"

glm::vec4 Rendering::DefaultShader::VertexPass(const Geometry::Vertex& p_vertex)
{
	const glm::mat4 u_Model      = GetUniformAs<glm::mat4>("u_Model");
	const glm::mat4 u_View       = GetUniformAs<glm::mat4>("u_View");
	const glm::mat4 u_Projection = GetUniformAs<glm::mat4>("u_Projection");

	glm::vec3 fragPos = glm::vec3(u_Model * glm::vec4(p_vertex.position, 1.0));
	SetFlat<glm::vec3>("v_FragPos", fragPos);

	glm::vec3 normal = glm::mat3(glm::transpose(glm::inverse(u_Model))) * p_vertex.normal;
	SetVarying<glm::vec3>("v_Normal", normal);

	glm::vec2 texCoords = p_vertex.textCoords;
	SetVarying<glm::vec2>("v_TextCoords", texCoords);

	return  u_Projection * u_View * glm::vec4(fragPos, 1.0);
}

Data::Color Rendering::DefaultShader::FragmentPass()
{
	glm::vec3 normal     = glm::normalize(GetVaryingAs<glm::vec3>("v_Normal"));
	glm::vec2 textCoords = GetVaryingAs<glm::vec2>("v_TextCoords");

	//return glm::vec4(((normal * 0.5f) + 0.5f), 1.0f);

	auto texture = GetSample("u_DiffuseMap");

	glm::vec4 diffuse = Texture(*texture, textCoords);

	return diffuse;
}
