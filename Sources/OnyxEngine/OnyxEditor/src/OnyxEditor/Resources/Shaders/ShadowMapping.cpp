#include "OnyxEditor/Resources/Shaders/ShadowMapping.h"

#include "OnyxEditor/Resources/Shaders/PBRUtils.h"

OnyxEditor::Resources::Shaders::ShadowMapping::ShadowMapping()
{
	RegisterUniform("u_DiffuseMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 0 });
	RegisterUniform("u_MetallicRoughnessMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 1 });
	RegisterUniform("u_NormalMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 2 });
	RegisterUniform("u_EmissiveMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 3 });
	RegisterUniform("u_AOMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 4 });
	RegisterUniform("u_DepthMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 5 });
	RegisterUniform("u_DisplacementMap", AmberGL::SoftwareRenderer::Programs::Sampler2D{ 6 });
	RegisterUniform("u_EmissiveStrength", 0.0f);
	RegisterUniform("u_LightPosition", glm::vec3(3.0f, 4.0f, 5.0f));
	RegisterUniform("u_LightDirection", glm::vec3(0.0f, -1.0f, 0.0f)); // NEW: Light view direction
	RegisterUniform("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	RegisterUniform("u_LightIntensity", 40.0f);

	RegisterUniform("u_LightSpaceMatrix", glm::mat4(1.0f));
	RegisterUniform("u_ShadowMapSize", 2048.0f);
	RegisterUniform("u_PCFRadius", 1);
	RegisterUniform("u_ShadowBiasMin", 0.005f);
	RegisterUniform("u_ShadowBiasMax", 0.05f);

	RegisterUniform("u_NormalStrength", 2.0f);

	RegisterUniform("u_DisplacementScale", 0.05f);
	RegisterUniform("u_ParallaxMethod", 2);
	RegisterUniform("u_ParallaxMinLayers", 8);
	RegisterUniform("u_ParallaxMaxLayers", 32);
	RegisterUniform("u_ParallaxEdgeClamp", 1);

	RegisterUniform("u_AmbientStrength", 0.02f);
	RegisterUniform("u_AmbientColor", glm::vec3(1.0f, 1.0f, 1.0f));

	RegisterUniform("u_UseFilmicCurve", 0);
}

std::unique_ptr<AmberGL::SoftwareRenderer::Programs::AProgram>OnyxEditor::Resources::Shaders::ShadowMapping::clone() const
{
	return std::make_unique<ShadowMapping>(*this);
}

glm::vec4 OnyxEditor::Resources::Shaders::ShadowMapping::VertexPass(const AmberGL::Geometry::Vertex& p_vertex)
{
	const glm::mat4 u_Model = GetUBOModelMatrix();
	const glm::mat4 u_View = GetUBOViewMatrix();
	const glm::mat4 u_Projection = GetUBOProjectionMatrix();
	const glm::mat4 u_LightSpaceMatrix = GetUniformAs<glm::mat4>("u_LightSpaceMatrix");

	glm::vec3 fragPos = glm::vec3(u_Model * glm::vec4(p_vertex.position, 1.0));
	SetVarying<glm::vec3>("v_FragPos", fragPos);

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(u_Model)));
	glm::vec3 worldNormal = glm::normalize(normalMatrix * p_vertex.normal);
	SetVarying<glm::vec3>("v_Normal", worldNormal);

	SetVarying<glm::vec2>("v_TexCoords", p_vertex.texCoords);

	glm::vec3 worldTangent = glm::normalize(normalMatrix * p_vertex.tangent);
	SetVarying<glm::vec3>("v_Tangent", worldTangent);

	glm::vec4 fragPosLightSpace = u_LightSpaceMatrix * glm::vec4(fragPos, 1.0);
	SetVarying<glm::vec4>("v_FragPosLightSpace", fragPosLightSpace);

	return u_Projection * u_View * glm::vec4(fragPos, 1.0);
}

glm::vec4 OnyxEditor::Resources::Shaders::ShadowMapping::FragmentPass()
{
	const glm::vec2 texCoords = GetVaryingAs<glm::vec2>("v_TexCoords");
	const glm::vec3 fragPos = GetVaryingAs<glm::vec3>("v_FragPos");
	const glm::vec3 normal = glm::normalize(GetVaryingAs<glm::vec3>("v_Normal"));
	const glm::vec3 tangent = glm::normalize(GetVaryingAs<glm::vec3>("v_Tangent"));
	const glm::vec4 fragPosLightSpace = GetVaryingAs<glm::vec4>("v_FragPosLightSpace");

	glm::vec3 T = glm::normalize(tangent - glm::dot(tangent, normal) * normal);
	glm::vec3 B = glm::normalize(glm::cross(normal, T));
	glm::mat3 TBN = glm::mat3(T, B, normal);
	glm::mat3 TBNInverse = glm::transpose(TBN);

	const glm::vec3 camPos = GetUBOCameraPosition();
	glm::vec3 viewDirTangent = glm::normalize(TBNInverse * (camPos - fragPos));

	const glm::vec3 lightPosition = GetUniformAs<glm::vec3>("u_LightPosition");
	const glm::vec3 lightDirection = GetUniformAs<glm::vec3>("u_LightDirection");
	const glm::vec3 lightColor = GetUniformAs<glm::vec3>("u_LightColor");
	const float lightIntensity = GetUniformAs<float>("u_LightIntensity");
	const float normalStrength = GetUniformAs<float>("u_NormalStrength");
	const float ambientStrength = GetUniformAs<float>("u_AmbientStrength");
	const glm::vec3 ambientColor = GetUniformAs<glm::vec3>("u_AmbientColor");
	const float shadowMapSize = GetUniformAs<float>("u_ShadowMapSize");
	const int pcfRadius = GetUniformAs<int>("u_PCFRadius");
	const float shadowBiasMin = GetUniformAs<float>("u_ShadowBiasMin");
	const float shadowBiasMax = GetUniformAs<float>("u_ShadowBiasMax");
	const float displacementScale = GetUniformAs<float>("u_DisplacementScale");
	const int parallaxMethod = GetUniformAs<int>("u_ParallaxMethod");
	const int parallaxMinLayers = GetUniformAs<int>("u_ParallaxMinLayers");
	const int parallaxMaxLayers = GetUniformAs<int>("u_ParallaxMaxLayers");
	const int parallaxEdgeClamp = GetUniformAs<int>("u_ParallaxEdgeClamp");
	const float emissiveStrength = GetUniformAs<float>("u_EmissiveStrength");
	const int useFilmicCurve = GetUniformAs<int>("u_UseFilmicCurve");

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

	float shadow = CalculateShadow(fragPosLightSpace, normal, lightDirection, shadowMapSize, pcfRadius, shadowBiasMin, shadowBiasMax);

	Lo *= (1.0f - shadow);

	glm::vec3 ambient = ambientColor * ambientStrength * albedo * ao;

	glm::vec3 emissive = glm::vec3(Texture("u_EmissiveMap", finalTexCoords)) * emissiveStrength;

	glm::vec3 color = ambient + Lo + emissive;

	color = PBRUtils::ToneMapReinhard(color);

	if (useFilmicCurve != 0)
	{
		color = PBRUtils::ApplyFilmicCurve(color);
	}

	color = PBRUtils::GammaCorrect(color);

	return glm::vec4(color, 1.0f);
}

float OnyxEditor::Resources::Shaders::ShadowMapping::CalculateShadow(const glm::vec4& p_fragPosLightSpace, const glm::vec3& p_geometricNormal, const glm::vec3& p_lightDir, float p_shadowMapSize, int p_pcfRadius, float p_minBias, float p_maxBias) const
{
	glm::vec3 projCoords = glm::vec3(p_fragPosLightSpace) / p_fragPosLightSpace.w;

	projCoords = projCoords * 0.5f + 0.5f;

	if (projCoords.z > 1.0f)
	{
		return 0.0f;
	}

	float currentDepth = projCoords.z;

	float cosTheta = glm::clamp(glm::dot(p_geometricNormal, -p_lightDir), 0.0f, 1.0f);

	float bias = glm::max(p_maxBias * (1.0f - cosTheta), p_minBias);

	float shadow = 0.0f;
	glm::vec2 texelSize = glm::vec2(1.0f / p_shadowMapSize);

	for (int x = -p_pcfRadius; x <= p_pcfRadius; ++x)
	{
		for (int y = -p_pcfRadius; y <= p_pcfRadius; ++y)
		{
			glm::vec2 offset = glm::vec2(x, y) * texelSize;
			glm::vec2 sampleCoords = glm::vec2(projCoords.x, projCoords.y) + offset;

			float pcfDepth = Texture("u_DepthMap", sampleCoords).x;

			shadow += (currentDepth - bias > pcfDepth) ? 1.0f : 0.0f;
		}
	}

	int totalSamples = (2 * p_pcfRadius + 1) * (2 * p_pcfRadius + 1);
	shadow /= static_cast<float>(totalSamples);

	return shadow;
}