#include "Data/Color.h"

const Data::Color Data::Color::Red   = { 255, 0, 0 };
const Data::Color Data::Color::Green = { 0, 255, 0 };
const Data::Color Data::Color::Blue  = { 0, 0, 255 };
const Data::Color Data::Color::White = { 255, 255, 255 };

Data::Color::Color(uint8_t p_r, uint8_t p_g, uint8_t p_b, uint8_t p_a) : r(p_r), g(p_g), b(p_b), a(p_a)
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
