#include "AmberRenderer/Rendering/Rasterizer/Shaders/ShadowMapping.h"

glm::vec4 AmberRenderer::Rendering::Rasterizer::Shaders::ShadowMapping::VertexPass(const Geometry::Vertex& p_vertex)
{
	const glm::mat4 u_Model            = GetUniformAs<glm::mat4>("u_Model");
	const glm::mat4 u_View             = GetUniformAs<glm::mat4>("u_View");
	const glm::mat4 u_Projection       = GetUniformAs<glm::mat4>("u_Projection");
	const glm::mat4 u_lightSpaceMatrix = GetUniformAs<glm::mat4>("u_lightSpaceMatrix");

	const glm::vec3 fragPos = glm::vec3(u_Model * glm::vec4(p_vertex.position, 1.0));
	SetVarying<glm::vec3>("v_FragPos", fragPos);

	const glm::vec3 normal = glm::mat3(glm::transpose(glm::inverse(u_Model))) * p_vertex.normal;
	SetVarying<glm::vec3>("v_Normal", normal);

	SetVarying<glm::vec2>("v_TexCoords", p_vertex.texCoords);

	const glm::vec4 fragPosLightSpace = u_lightSpaceMatrix * glm::vec4(fragPos, 1.0);

	SetVarying<glm::vec4>("v_FragPosLightSpace", fragPosLightSpace);

	return  u_Projection * u_View * glm::vec4(fragPos, 1.0);
}

constexpr glm::vec3 LightDiffuse = glm::vec3(1.0f);
constexpr glm::vec3 LightAmbient = glm::vec3(0.1f);

AmberRenderer::Data::Color AmberRenderer::Rendering::Rasterizer::Shaders::ShadowMapping::FragmentPass()
{
	const glm::vec3 normal = glm::normalize(GetVaryingAs<glm::vec3>("v_Normal"));
	const glm::vec2 texCoords = GetVaryingAs<glm::vec2>("v_TexCoords");
	const glm::vec3 fragPos = GetVaryingAs<glm::vec3>("v_FragPos");
	const glm::vec3 lightPos = GetUniformAs<glm::vec3>("u_lightPos");

	auto u_DiffuseMap = GetSample("u_DiffuseMap");
	const glm::vec4 diffuseTexel = Texture(*u_DiffuseMap, texCoords);

	const glm::vec4 fragPosLightSpace = GetVaryingAs<glm::vec4>("v_FragPosLightSpace");
	glm::vec3 projCoords = glm::vec3(fragPosLightSpace) / fragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;

	auto u_DepthMap = GetSample("u_DepthMap");
	float closestDepth = Texture(*u_DepthMap, glm::vec2(projCoords.x, projCoords.y)).x;
	float currentDepth = projCoords.z;
	float bias = glm::max(0.05f * (1.0f - dot(normal, glm::vec3(0, 1, 0))), 0.005f);
	float shadow = (currentDepth - bias > closestDepth) ? 1.0f : 0.0f;

	glm::vec3 lightDir = glm::normalize(lightPos - fragPos);
	float diff = glm::max(glm::dot(normal, lightDir), 0.0f);
	glm::vec3 diffuse = LightDiffuse * diff;
	glm::vec3 ambient = LightAmbient;
	glm::vec3 lighting = ambient + diffuse;

	glm::vec3 finalColor = lighting * glm::vec3(diffuseTexel) * (1.0f - shadow);

	finalColor = glm::clamp(finalColor, 0.0f, 1.0f);

	return glm::vec4(finalColor, 1.0f);
}
