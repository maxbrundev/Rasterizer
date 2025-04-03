#include "AmberGL/SoftwareRenderer/Programs/ShadowMapping.h"

glm::vec4 AmberGL::SoftwareRenderer::Programs::ShadowMapping::VertexPass(const Geometry::Vertex& p_vertex)
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
constexpr glm::vec3 LightAmbient = glm::vec3(0.3f);

AmberGL::Data::Color AmberGL::SoftwareRenderer::Programs::ShadowMapping::FragmentPass()
{
	const glm::vec3 normal = glm::normalize(GetVaryingAs<glm::vec3>("v_Normal"));
	const glm::vec2 texCoords = GetVaryingAs<glm::vec2>("v_TexCoords");
	const glm::vec3 fragPos = GetVaryingAs<glm::vec3>("v_FragPos");
	const glm::vec3 lightPos = GetUniformAs<glm::vec3>("u_lightPos");

	//auto u_DiffuseMap = GetSample("u_DiffuseMap");
	const glm::vec4 diffuseTexel = Texture("u_DiffuseMap", texCoords);



	const glm::vec4 fragPosLightSpace = GetVaryingAs<glm::vec4>("v_FragPosLightSpace");
	glm::vec3 projCoords = glm::vec3(fragPosLightSpace) / fragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;

	//auto u_DepthMap = GetSample("u_DepthMap");
	float closestDepth = Texture("u_DepthMap", glm::vec2(projCoords.x, projCoords.y)).x;
	float currentDepth = projCoords.z;
	float bias = glm::max(0.05f * (1.0f - dot(normal, glm::vec3(0, 1, 0))), 0.005f);


	float shadow = 0.0f;

	glm::vec2 texelSize = glm::vec2(1.0f /( 1024 * 1024));
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = Texture("u_DepthMap", glm::vec2(projCoords.x, projCoords.y) + glm::vec2(x, y) * texelSize).x;
			shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;
		}
	}
	shadow /= 9.0f;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if (projCoords.z > 1.0)
		shadow = 0.0;

	//float shadow = (currentDepth - bias > closestDepth) ? 1.0f : 0.0f;

	glm::vec3 lightDir = glm::normalize(lightPos - fragPos);
	float diff = glm::max(glm::dot(normal, lightDir), 0.0f);
	glm::vec3 diffuse = LightDiffuse * diff;

	glm::vec3 finalColor = diffuse * glm::vec3(diffuseTexel) *  (LightAmbient + (1.0f - shadow));

	finalColor = glm::clamp(finalColor, 0.0f, 1.0f);

	return glm::vec4(finalColor, 1.0f);
}
