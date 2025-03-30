#include "AmberRenderer/Rendering/Driver.h"

#include "AmberRenderer/Rendering/SoftwareRenderer/AmberGL.h"

AmberRenderer::Rendering::Driver::Driver()
{
	Initialize();
}

void AmberRenderer::Rendering::Driver::SetClearColor(const glm::vec4& p_color) const
{
	AmberGL::ClearColor(p_color.x, p_color.y, p_color.z, p_color.w);
}

void AmberRenderer::Rendering::Driver::Clear(bool p_colorBuffer, bool p_depthBuffer) const
{
	uint8_t clearMask = 0;

	if (p_colorBuffer) clearMask |= AGL_COLOR_BUFFER_BIT;
	if (p_depthBuffer) clearMask |= AGL_DEPTH_BUFFER_BIT;

	if (clearMask != 0)
	{
		AmberGL::Clear(clearMask);
	}
}

void AmberRenderer::Rendering::Driver::SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const
{
	AmberGL::Viewport(p_x, p_y, p_width, p_height);
}

void AmberRenderer::Rendering::Driver::SetRasterizationMode(Settings::ERasterizationMode p_rasterizationMode) const
{
	AmberGL::PolygonMode(static_cast<uint8_t>(p_rasterizationMode));
}

void AmberRenderer::Rendering::Driver::SetCapability(Settings::ERenderingCapability p_capability, bool p_value) const
{
	(p_value ? AmberGL::Enable : AmberGL::Disable)(static_cast<uint8_t>(p_capability));
}

bool AmberRenderer::Rendering::Driver::GetCapability(Settings::ERenderingCapability p_capability) const
{
	return AmberGL::IsEnabled(static_cast<uint8_t>(p_capability));
}

void AmberRenderer::Rendering::Driver::SetCullFace(Settings::ECullFace p_cullFace) const
{
	AmberGL::CullFace(static_cast<uint8_t>(p_cullFace));
}

void AmberRenderer::Rendering::Driver::SetDepthWriting(bool p_enable) const
{
	AmberGL::DepthMask(p_enable);
}

bool AmberRenderer::Rendering::Driver::GetBool(uint8_t p_parameter) const
{
	bool result;
	AmberGL::GetBool(p_parameter, &result);
	return result;
}

int AmberRenderer::Rendering::Driver::GetInt(uint8_t p_parameter) const
{
	int result;
	AmberGL::GetInt(p_parameter, &result);
	return result;
}

void AmberRenderer::Rendering::Driver::Initialize() const
{
	AmberGL::Initialize(800, 600);
}
