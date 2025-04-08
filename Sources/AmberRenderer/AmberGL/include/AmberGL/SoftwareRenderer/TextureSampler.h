#pragma once

#include <array>
#include <glm/glm.hpp>

#include "RenderObject/TextureObject.h"

namespace AmberGL::SoftwareRenderer
{
	class TextureSampler
	{
	public:
		TextureSampler() = default;
		~TextureSampler() = default;

		static glm::vec4 Sample(const RenderObject::TextureObject* p_textureObject, const glm::vec2& p_texCoords, const std::array<glm::vec2, 2>& p_texCoordsDerivatives);
		static uint8_t ComputeMipmapLevel(const RenderObject::TextureObject* p_textureObject, const glm::vec2& p_dfdx, const glm::vec2& p_dfdy);
	private:
		static float ApplyWrapMode(float p_coord, uint8_t p_wrapMode);

		static glm::vec4 SampleNearest(const uint8_t* p_data, uint32_t p_width, uint32_t p_height, float p_x, float p_y);
		static glm::vec4 SampleBilinear(const uint8_t* p_data, uint32_t p_width, glm::uint32_t p_height, float p_x, float p_y);
	};
}
