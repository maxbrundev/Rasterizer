#pragma once

#include <AmberGL/SoftwareRenderer/Programs/AProgram.h>

namespace OnyxEditor::Resources::Shaders
{
	class PBRUtils
	{
	public:
		enum class ParallaxMethod
		{
			None,
			Simple,
			Steep,
			ParallaxOcclusion
		};

		static float DistributionGGX(const glm::vec3& p_surfaceNormal, const glm::vec3& p_halfVector, float p_roughness);

		static float GeometrySchlickGGX(float p_normalDotView, float p_roughness);

		static float GeometrySmith(const glm::vec3& p_surfaceNormal, const glm::vec3& p_viewDirection, const glm::vec3& p_lightDirection, float p_roughness);

		static glm::vec3 FresnelSchlick(float p_cosAngle, const glm::vec3& p_baseReflectance);

		static glm::vec2 CalculateParallaxOffset(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, const glm::vec3& p_viewDirection, ParallaxMethod p_method, float p_heightScale, int p_minLayers = 8, int p_maxLayers = 32);

		static glm::vec2 ParallaxSimple(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, const glm::vec3& p_viewDirection, float p_heightScale);

		static glm::vec2 ParallaxSteep(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, const glm::vec3& p_viewDirection, float p_heightScale, int p_minLayers, int p_maxLayers);

		static glm::vec2 ParallaxOcclusion(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, const glm::vec3& p_viewDirection, float p_heightScale, int p_minLayers, int p_maxLayers);

		static void SamplePBRTextures(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, glm::vec3& p_albedo, float& p_metallic, float& p_roughness, float& p_ambientOcclusion);

		static glm::vec3 ApplyNormalMapping(const AmberGL::SoftwareRenderer::Programs::AProgram* p_program, const glm::vec2& p_texCoords, const glm::vec3& p_surfaceNormal, const glm::vec3& p_tangent, float p_normalStrength);

		static glm::vec3 CalculatePBRLighting(const glm::vec3& p_surfaceNormal, const glm::vec3& p_viewDirection, const glm::vec3& p_lightDirection, const glm::vec3& p_halfVector, const glm::vec3& p_albedo, float p_metallic, float p_roughness, const glm::vec3& p_radiance);

		static glm::vec3 ToneMapReinhard(const glm::vec3& p_color);

		static glm::vec3 GammaCorrect(const glm::vec3& p_color);

		static glm::vec3 ApplyFilmicCurve(const glm::vec3& p_color);
	};
}