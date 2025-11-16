#include "OnyxEditor/Resources/Shaders/StandardPBRShader.h"

#include "OnyxEditor/Resources/Shaders/PBRUtils.h"

OnyxEditor::Resources::Shaders::StandardPBRShader::StandardPBRShader()
{
	RegisterUniform("u_DiffuseMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 0 });
	RegisterUniform("u_MetallicRoughnessMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 1 });
	RegisterUniform("u_NormalMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 2 });
	RegisterUniform("u_EmissiveMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 3 });
	RegisterUniform("u_AOMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 4 });
	RegisterUniform("u_DisplacementMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 5 });
	RegisterUniform("u_EmissiveStrength", 0.0f);
	RegisterUniform("u_LightPosition", glm::vec3(3.0f, 4.0f, 5.0f));
	RegisterUniform("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	RegisterUniform("u_LightIntensity", 40.0f);

	RegisterUniform("u_NormalStrength", 2.0f);

	RegisterUniform("u_DisplacementScale", 0.05f);
	RegisterUniform("u_ParallaxMethod", 2);
	RegisterUniform("u_ParallaxMinLayers", 8);
	RegisterUniform("u_ParallaxMaxLayers", 32);
	RegisterUniform("u_ParallaxEdgeClamp", 1);

	RegisterUniform("u_AmbientStrength", 0.02f);
	RegisterUniform("u_AmbientColor", glm::vec3(1.0f, 1.0f, 1.0f));
}

glm::vec4 OnyxEditor::Resources::Shaders::StandardPBRShader::VertexPass(
	const AmberGL::Geometry::Vertex& p_vertex)
{
	const glm::mat4 u_Model = GetUBOModelMatrix();
	const glm::mat4 u_View = GetUBOViewMatrix();
	const glm::mat4 u_Projection = GetUBOProjectionMatrix();

	glm::vec3 fragPos = glm::vec3(u_Model * glm::vec4(p_vertex.position, 1.0));
	SetVarying<glm::vec3>("v_FragPos", fragPos);

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(u_Model)));
	glm::vec3 worldNormal = glm::normalize(normalMatrix * p_vertex.normal);
	SetVarying<glm::vec3>("v_Normal", worldNormal);

	SetVarying<glm::vec2>("v_TexCoords", p_vertex.texCoords);

	glm::vec3 worldTangent = glm::normalize(normalMatrix * p_vertex.tangent);
	SetVarying<glm::vec3>("v_Tangent", worldTangent);

	return u_Projection * u_View * glm::vec4(fragPos, 1.0);
}

glm::vec4 OnyxEditor::Resources::Shaders::StandardPBRShader::FragmentPass()
{
	const glm::vec2 texCoords = GetVaryingAs<glm::vec2>("v_TexCoords");
	const glm::vec3 fragPos = GetVaryingAs<glm::vec3>("v_FragPos");
	const glm::vec3 normal = glm::normalize(GetVaryingAs<glm::vec3>("v_Normal"));
	const glm::vec3 tangent = glm::normalize(GetVaryingAs<glm::vec3>("v_Tangent"));

	glm::vec3 T = glm::normalize(tangent - glm::dot(tangent, normal) * normal);
	glm::vec3 B = glm::normalize(glm::cross(normal, T));
	glm::mat3 TBN = glm::mat3(T, B, normal);
	glm::mat3 TBNInverse = glm::transpose(TBN);

	const glm::vec3 camPos = GetUBOCameraPosition();
	glm::vec3 viewDirTangent = glm::normalize(TBNInverse * (camPos - fragPos));

	const glm::vec3 lightPosition = GetUniformAs<glm::vec3>("u_LightPosition");
	const glm::vec3 lightColor = GetUniformAs<glm::vec3>("u_LightColor");
	const float lightIntensity = GetUniformAs<float>("u_LightIntensity");
	const float normalStrength = GetUniformAs<float>("u_NormalStrength");
	const float ambientStrength = GetUniformAs<float>("u_AmbientStrength");
	const glm::vec3 ambientColor = GetUniformAs<glm::vec3>("u_AmbientColor");
	const float displacementScale = GetUniformAs<float>("u_DisplacementScale");
	const int parallaxMethod = GetUniformAs<int>("u_ParallaxMethod");
	const int parallaxMinLayers = GetUniformAs<int>("u_ParallaxMinLayers");
	const int parallaxMaxLayers = GetUniformAs<int>("u_ParallaxMaxLayers");
	const int parallaxEdgeClamp = GetUniformAs<int>("u_ParallaxEdgeClamp");
	const float emissiveStrength = GetUniformAs<float>("u_EmissiveStrength");
	
	glm::vec2 finalTexCoords = texCoords;

	if (displacementScale > 0.0f)
	{
		PBRUtils::ParallaxMethod method = static_cast<PBRUtils::ParallaxMethod>(parallaxMethod);
		finalTexCoords = PBRUtils::CalculateParallaxOffset(this, texCoords, viewDirTangent, method, displacementScale, parallaxMinLayers, parallaxMaxLayers);

		if (parallaxEdgeClamp != 0)
		{
			finalTexCoords = glm::clamp(finalTexCoords, glm::vec2(0.0f), glm::vec2(1.0f));
		}
		else
		{
			if (finalTexCoords.x < 0.0f || finalTexCoords.x > 1.0f || finalTexCoords.y < 0.0f || finalTexCoords.y > 1.0f)
			{
				return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			}
		}
	}

	glm::vec3 albedo;
	float metallic;
	float roughness;
	float ao;

	PBRUtils::SamplePBRTextures(this, finalTexCoords, albedo, metallic, roughness, ao);

	glm::vec3 N = PBRUtils::ApplyNormalMapping(this, finalTexCoords, normal, tangent, normalStrength);

	const glm::vec3 V = glm::normalize(camPos - fragPos);
	const glm::vec3 L = glm::normalize(lightPosition - fragPos);
	const glm::vec3 H = glm::normalize(V + L);

	float distance = glm::length(lightPosition - fragPos);
	float attenuation = 1.0f / (distance * distance);
	glm::vec3 radiance = lightColor * lightIntensity * attenuation;

	glm::vec3 Lo = PBRUtils::CalculatePBRLighting(N, V, L, H, albedo, metallic, roughness, radiance);

	glm::vec3 ambient = ambientColor * ambientStrength * albedo * ao;

	glm::vec3 emissive = glm::vec3(Texture("u_EmissiveMap", finalTexCoords)) * emissiveStrength;

	glm::vec3 color = ambient + Lo + emissive;

	color = PBRUtils::ToneMapReinhard(color);

	color = PBRUtils::GammaCorrect(color);

	return glm::vec4(color, 1.0f);
}

std::unique_ptr<AmberGL::SoftwareRenderer::Programs::AProgram>OnyxEditor::Resources::Shaders::StandardPBRShader::clone() const
{
	return std::make_unique<StandardPBRShader>(*this);
}