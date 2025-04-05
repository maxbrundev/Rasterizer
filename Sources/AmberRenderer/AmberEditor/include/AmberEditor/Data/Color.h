#pragma once

#include <cstdint>
#include <tuple>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace AmberEditor::Data
{
	struct Color
	{
		static const Color Red;
		static const Color Green;
		static const Color Blue;
		static const Color White;

		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		Color(uint8_t p_r = 255, uint8_t p_g = 255, uint8_t p_b = 255, uint8_t p_a = 255);
		Color(uint32_t p_packedData);
		Color(const glm::vec3& p_normalizedColor);
		Color(const glm::vec4& p_normalizedColor);

		uint32_t Pack() const;

		std::tuple<float, float, float, float> GetNormalized() const;

		glm::vec3 GetNormalizedVec3() const;
		glm::vec4 GetNormalizedVec4() const;

		Color& operator+=(const Color& p_color);

		static Color Mix(const Color& p_color1, const Color& p_color2, float p_alpha);\
	};
}
