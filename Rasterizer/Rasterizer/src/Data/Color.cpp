#include "Data/Color.h"

const Data::Color Data::Color::Red   = { 255, 0, 0 };
const Data::Color Data::Color::Green = { 0, 255, 0 };
const Data::Color Data::Color::Blue  = { 0, 0, 255 };
const Data::Color Data::Color::White = { 255, 255, 255 };

Data::Color::Color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a) : r(p_r), g(p_g), b(p_b), a(p_a)
{
}

Data::Color::Color(const glm::vec3& p_normalizedColor) :
	Color
	(
		static_cast<uint8_t>(p_normalizedColor.x * 255.0f),
		static_cast<uint8_t>(p_normalizedColor.y * 255.0f),
		static_cast<uint8_t>(p_normalizedColor.z * 255.0f)
	)
{
}

Data::Color::Color(const glm::vec4& p_normalizedColor) :
	Color
	(
		static_cast<uint8_t>(p_normalizedColor.x * 255.0f),
		static_cast<uint8_t>(p_normalizedColor.y * 255.0f),
		static_cast<uint8_t>(p_normalizedColor.z * 255.0f),
		static_cast<uint8_t>(p_normalizedColor.w * 255.0f)
	)
{
}

uint32_t Data::Color::Pack() const
{
	return (r << 24) | (g << 16) | (b << 8) | a;
}

std::tuple<float, float, float, float> Data::Color::GetNormalized() const
{
	return std::make_tuple(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

glm::vec3 Data::Color::GetNormalizedVec3() const
{
	return glm::vec3(r / 255.f, g / 255.f, b / 255.f);
}

glm::vec4 Data::Color::GetNormalizedVec4() const
{
	return glm::vec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}
