#include "OnyxEditor/Resources/Shaders/PBRUtils.h"

#include <glm/ext/scalar_constants.hpp>

float OnyxEditor::Resources::Shaders::PBRUtils::DistributionGGX(const glm::vec3& p_surfaceNormal, const glm::vec3& p_halfVector, float p_roughness)
{
	float alphaRoughness = p_roughness * p_roughness;
	float alphaSquared = alphaRoughness * alphaRoughness;
	float normalDotHalf = glm::max(glm::dot(p_surfaceNormal, p_halfVector), 0.0f);
	float normalDotHalfSquared = normalDotHalf * normalDotHalf;

	float numerator = alphaSquared;
	float denominator = (normalDotHalfSquared * (alphaSquared - 1.0f) + 1.0f);
	denominator = glm::pi<float>() * denominator * denominator;

	return numerator / glm::max(denominator, glm::epsilon<float>());
}

float OnyxEditor::Resources::Shaders::PBRUtils::GeometrySchlickGGX(float p_normalDotView, float p_roughness)
{
	float remappedRoughness = (p_roughness + 1.0f);
	float geometryFactor = (remappedRoughness * remappedRoughness) / 8.0f;

	float numerator = p_normalDotView;
	float denominator = p_normalDotView * (1.0f - geometryFactor) + geometryFactor;

	return numerator / glm::max(denominator, glm::epsilon<float>());
}

float OnyxEditor::Resources::Shaders::PBRUtils::GeometrySmith(const glm::vec3& p_surfaceNormal, const glm::vec3& p_viewDirection, const glm::vec3& p_lightDirection, float p_roughness)
{
	float normalDotView = glm::max(glm::dot(p_surfaceNormal, p_viewDirection), 0.0f);
	float normalDotLight = glm::max(glm::dot(p_surfaceNormal, p_lightDirection), 0.0f);
	float geometryViewTerm = GeometrySchlickGGX(normalDotView, p_roughness);
	float geometryLightTerm = GeometrySchlickGGX(normalDotLight, p_roughness);

	return geometryLightTerm * geometryViewTerm;
}

glm::vec3 OnyxEditor::Resources::Shaders::PBRUtils::FresnelSchlick(float p_cosAngle, const glm::vec3& p_baseReflectance)
{
	return p_baseReflectance + (glm::vec3(1.0f) - p_baseReflectance) * glm::pow(glm::clamp(1.0f - p_cosAngle, 0.0f, 1.0f), 5.0f);
}

glm::vec2 OnyxEditor::Resources::Shaders::PBRUtils::CalculateParallaxOffset(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, const glm::vec3& p_viewDirection, ParallaxMethod p_method, float p_heightScale, int p_minLayers, int p_maxLayers)
{
	if (p_heightScale <= 0.0f)
		return p_texCoords;

	switch (p_method)
	{
	case ParallaxMethod::Simple:
		return ParallaxSimple(p_program, p_texCoords, p_viewDirection, p_heightScale);

	case ParallaxMethod::Steep:
		return ParallaxSteep(p_program, p_texCoords, p_viewDirection, p_heightScale, p_minLayers, p_maxLayers);

	case ParallaxMethod::ParallaxOcclusion:
		return ParallaxOcclusion(p_program, p_texCoords, p_viewDirection, p_heightScale, p_minLayers, p_maxLayers);

	case ParallaxMethod::None: default:
		return p_texCoords;
	}
}

glm::vec2 OnyxEditor::Resources::Shaders::PBRUtils::ParallaxSimple(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, const glm::vec3& p_viewDirection, float p_heightScale)
{
	float height = p_program->Texture("u_DisplacementMap", p_texCoords).r;

	glm::vec2 offset = glm::vec2(p_viewDirection.x, p_viewDirection.y) * (height * p_heightScale);

	return p_texCoords - offset;
}

glm::vec2 OnyxEditor::Resources::Shaders::PBRUtils::ParallaxSteep(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, const glm::vec3& p_viewDirection, float p_heightScale, int p_minLayers, int p_maxLayers)
{
	float numLayers = glm::mix(static_cast<float>(p_maxLayers), static_cast<float>(p_minLayers), glm::abs(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), p_viewDirection)));

	float layerDepth = 1.0f / numLayers;
	float currentLayerDepth = 0.0f;

	glm::vec2 deltaTexCoords = glm::vec2(p_viewDirection.x, p_viewDirection.y) * p_heightScale / numLayers;

	glm::vec2 currentTexCoords = p_texCoords;
	float currentDepthMapValue = p_program->Texture("u_DisplacementMap", currentTexCoords).r;

	while (currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = p_program->Texture("u_DisplacementMap", currentTexCoords).r;
		currentLayerDepth += layerDepth;
	}

	return currentTexCoords;
}

glm::vec2 OnyxEditor::Resources::Shaders::PBRUtils::ParallaxOcclusion(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, const glm::vec3& p_viewDirection, float p_heightScale, int p_minLayers, int p_maxLayers)
{
	float numLayers = glm::mix(static_cast<float>(p_maxLayers), static_cast<float>(p_minLayers), glm::abs(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), p_viewDirection)));

	float layerDepth = 1.0f / numLayers;
	float currentLayerDepth = 0.0f;

	glm::vec2 deltaTexCoords = glm::vec2(p_viewDirection.x, p_viewDirection.y) * p_heightScale / numLayers;

	glm::vec2 currentTexCoords = p_texCoords;
	float currentDepthMapValue = p_program->Texture("u_DisplacementMap", currentTexCoords).r;

	while (currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = p_program->Texture("u_DisplacementMap", currentTexCoords).r;
		currentLayerDepth += layerDepth;
	}

	glm::vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = p_program->Texture("u_DisplacementMap", prevTexCoords).r - currentLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);

	glm::vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0f - weight);

	return finalTexCoords;
}

void OnyxEditor::Resources::Shaders::PBRUtils::SamplePBRTextures(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, glm::vec3& p_albedo, float& p_metallic, float& p_roughness, float& p_ambientOcclusion)
{
	glm::vec4 albedoSample = p_program->Texture("u_DiffuseMap", p_texCoords);
	p_albedo = glm::pow(glm::vec3(albedoSample), glm::vec3(2.2f));

	glm::vec4 metallicRoughnessSample = p_program->Texture("u_MetallicRoughnessMap", p_texCoords);
	p_metallic = metallicRoughnessSample.b;
	p_roughness = metallicRoughnessSample.g;

	p_ambientOcclusion = p_program->Texture("u_AOMap", p_texCoords).r;

	p_metallic = glm::clamp(p_metallic, 0.0f, 1.0f);
	p_roughness = glm::clamp(p_roughness, 0.04f, 1.0f);
	p_ambientOcclusion = glm::clamp(p_ambientOcclusion, 0.0f, 1.0f);
}

glm::vec3 OnyxEditor::Resources::Shaders::PBRUtils::ApplyNormalMapping(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, const glm::vec3& p_surfaceNormal, const glm::vec3& p_tangent, float p_normalStrength)
{
	glm::vec4 normalMapSample = p_program->Texture("u_NormalMap", p_texCoords);
	glm::vec3 normalMapRGB = glm::vec3(normalMapSample);

	glm::vec3 normalMapTangentSpace = normalMapRGB * 2.0f - 1.0f;

	normalMapTangentSpace.x *= p_normalStrength;
	normalMapTangentSpace.y *= p_normalStrength;

	float xyLengthSquared = normalMapTangentSpace.x * normalMapTangentSpace.x + normalMapTangentSpace.y * normalMapTangentSpace.y;
	normalMapTangentSpace.z = std::sqrt(glm::max(1.0f - xyLengthSquared, 0.0f));

	normalMapTangentSpace = glm::normalize(normalMapTangentSpace);

	glm::vec3 orthogonalTangent = glm::normalize(p_tangent - glm::dot(p_tangent, p_surfaceNormal) * p_surfaceNormal);
	glm::vec3 bitangent = glm::normalize(glm::cross(p_surfaceNormal, orthogonalTangent));
	glm::mat3 tangentToWorldMatrix(orthogonalTangent, bitangent, p_surfaceNormal);

	glm::vec3 resultNormal = glm::normalize(tangentToWorldMatrix * normalMapTangentSpace);

	return resultNormal;
}

glm::vec3 OnyxEditor::Resources::Shaders::PBRUtils::CalculatePBRLighting(const glm::vec3& p_surfaceNormal, const glm::vec3& p_viewDirection, const glm::vec3& p_lightDirection, const glm::vec3& p_halfVector, const glm::vec3& p_albedo, float p_metallic, float p_roughness, const glm::vec3& p_radiance)
{
	glm::vec3 baseReflectance = glm::mix(glm::vec3(0.04f), p_albedo, p_metallic);

	float normalDistribution = DistributionGGX(p_surfaceNormal, p_halfVector, p_roughness);
	float geometryTerm = GeometrySmith(p_surfaceNormal, p_viewDirection, p_lightDirection, p_roughness);
	glm::vec3 fresnelTerm = FresnelSchlick(glm::max(glm::dot(p_halfVector, p_viewDirection), 0.0f), baseReflectance);

	glm::vec3 numerator = normalDistribution * geometryTerm * fresnelTerm;
	float denominator = 4.0f * glm::max(glm::dot(p_surfaceNormal, p_viewDirection), 0.0f) * glm::max(glm::dot(p_surfaceNormal, p_lightDirection), 0.0f) + glm::epsilon<float>();
	glm::vec3 specularContribution = numerator / denominator;

	glm::vec3 specularRatio = fresnelTerm;
	glm::vec3 diffuseRatio = (glm::vec3(1.0f) - specularRatio) * (1.0f - p_metallic);

	float normalDotLight = glm::max(glm::dot(p_surfaceNormal, p_lightDirection), 0.0f);
	return (diffuseRatio * p_albedo / glm::pi<float>() + specularContribution) * p_radiance * normalDotLight;
}

glm::vec3 OnyxEditor::Resources::Shaders::PBRUtils::ToneMapReinhard(const glm::vec3& p_color)
{
	return p_color / (p_color + glm::vec3(1.0f));
}

glm::vec3 OnyxEditor::Resources::Shaders::PBRUtils::GammaCorrect(const glm::vec3& p_color)
{
	return glm::pow(p_color, glm::vec3(1.0f / 2.2f));
}

glm::vec3 OnyxEditor::Resources::Shaders::PBRUtils::ApplyFilmicCurve(const glm::vec3& p_color)
{
	// John Hable's Uncharted 2 filmic tone mapping curve
	glm::vec3 x = glm::max(glm::vec3(0.0f), p_color - 0.004f);
	return (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
}
