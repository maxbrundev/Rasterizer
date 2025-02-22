#include "Rendering/Driver.h"

#include "Rendering/GLRasterizer.h"

Rendering::Driver::Driver()
{
	Initialize();
}

void Rendering::Driver::SetRasterizationMode(Settings::ERasterizationMode p_rasterizationMode)
{
	GLRasterizer::PolygonMode(static_cast<uint8_t>(p_rasterizationMode));
}

void Rendering::Driver::SetCapability(Settings::ERenderingCapability p_capability, bool p_value)
{
	(p_value ? GLRasterizer::Enable : GLRasterizer::Disable)(static_cast<uint8_t>(p_capability));
}

bool Rendering::Driver::GetCapability(Settings::ERenderingCapability p_capability)
{
	return GLRasterizer::IsEnabled(static_cast<uint8_t>(p_capability));
}

void Rendering::Driver::SetCullFace(Settings::ECullFace p_cullFace)
{
	GLRasterizer::CullFace(static_cast<uint8_t>(p_cullFace));
}

void Rendering::Driver::SetDepthWriting(bool p_enable)
{
	GLRasterizer::DepthMask(p_enable);
}

bool Rendering::Driver::GetBool(uint8_t p_parameter)
{
	bool result;
	GLRasterizer::GetBool(p_parameter, &result);
	return result;
}

int Rendering::Driver::GetInt(uint8_t p_parameter)
{
	int result;
	GLRasterizer::GetInt(p_parameter, &result);
	return result;
}

void Rendering::Driver::Initialize()
{
	GLRasterizer::Initialize(800, 600);
}
