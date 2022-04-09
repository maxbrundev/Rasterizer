#pragma once

#include <cstdint>
#include <tuple>

namespace Data
{
	struct Color
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		Color(uint8_t p_r = 255, uint8_t p_g = 255, uint8_t p_b = 255, uint8_t p_a = 255);

		uint32_t Pack() const;

		std::tuple<float, float, float, float> GetNormalized() const;
	};
}

