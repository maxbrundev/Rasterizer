#include "AmberGL/Data/Color.h"

#include <glm/ext/quaternion_common.hpp>

const AmberGL::Data::Color AmberGL::Data::Color::Red   = { 255, 0, 0 };
const AmberGL::Data::Color AmberGL::Data::Color::Green = { 0, 255, 0 };
const AmberGL::Data::Color AmberGL::Data::Color::Blue  = { 0, 0, 255 };
const AmberGL::Data::Color AmberGL::Data::Color::White = { 255, 255, 255 };

AmberGL::Data::Color::Color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a) : r(p_r), g(p_g), b(p_b), a(p_a)
{
}

AmberGL::Data::Color::Color(uint32_t p_packedData) :
a(p_packedData),
b(p_packedData >> 8),
g(p_packedData >> 16),
r(p_packedData >> 24)
{
}

AmberGL::Data::Color::Color(const glm::vec3& p_normalizedColor) :
Color
(
	static_cast<uint8_t>(p_normalizedColor.x * 255.0f),
	static_cast<uint8_t>(p_normalizedColor.y * 255.0f),
	static_cast<uint8_t>(p_normalizedColor.z * 255.0f)
)
{
}

AmberGL::Data::Color::Color(const glm::vec4& p_normalizedColor) :
Color
(
	static_cast<uint8_t>(p_normalizedColor.x * 255.0f),
	static_cast<uint8_t>(p_normalizedColor.y * 255.0f),
	static_cast<uint8_t>(p_normalizedColor.z * 255.0f),
	static_cast<uint8_t>(p_normalizedColor.w * 255.0f)
)
{
}

uint32_t AmberGL::Data::Color::Pack() const
{
	return (r << 24) | (g << 16) | (b << 8) | a;
}

std::tuple<float, float, float, float> AmberGL::Data::Color::GetNormalized() const
{
	return std::make_tuple(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

glm::vec3 AmberGL::Data::Color::GetNormalizedVec3() const
{
	return glm::vec3(r / 255.f, g / 255.f, b / 255.f);
}

glm::vec4 AmberGL::Data::Color::GetNormalizedVec4() const
{
	return glm::vec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

AmberGL::Data::Color& AmberGL::Data::Color::operator+=(const Color& p_color)
{
	r += p_color.r;
	g += p_color.g;
	b += p_color.b;
	a += p_color.a;

	return *this;
}

AmberGL::Data::Color AmberGL::Data::Color::Mix(const Color& p_color1, const Color& p_color2, float p_alpha)
{
	return Data::Color(glm::mix(p_color1.GetNormalizedVec3(), p_color2.GetNormalizedVec3(), p_alpha));
}
