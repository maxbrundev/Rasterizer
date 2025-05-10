#include "AmberEditor/Rendering/Driver.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

AmberEditor::Rendering::Driver::Driver()
{
	Initialize();
}

void AmberEditor::Rendering::Driver::SetClearColor(const glm::vec4& p_color) const
{
	AmberGL::ClearColor(p_color.x, p_color.y, p_color.z, p_color.w);
}

void AmberEditor::Rendering::Driver::Clear(bool p_colorBuffer, bool p_depthBuffer) const
{
	uint8_t clearMask = 0;

	if (p_colorBuffer) clearMask |= AGL_COLOR_BUFFER_BIT;
	if (p_depthBuffer) clearMask |= AGL_DEPTH_BUFFER_BIT;

	if (clearMask != 0)
	{
		AmberGL::Clear(clearMask);
	}
}

void AmberEditor::Rendering::Driver::SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const
{
	AmberGL::Viewport(p_x, p_y, p_width, p_height);
}

void AmberEditor::Rendering::Driver::SetRasterizationMode(Settings::EPolygonMode p_rasterizationMode) const
{
	AmberGL::PolygonMode(static_cast<uint8_t>(p_rasterizationMode));
}

void AmberEditor::Rendering::Driver::SetCapability(Settings::ERenderingCapability p_capability, bool p_value) const
{
	(p_value ? AmberGL::Enable : AmberGL::Disable)(static_cast<uint8_t>(p_capability));
}

bool AmberEditor::Rendering::Driver::GetCapability(Settings::ERenderingCapability p_capability) const
{
	return AmberGL::IsEnabled(static_cast<uint8_t>(p_capability));
}

void AmberEditor::Rendering::Driver::SetCullFace(Settings::ECullFace p_cullFace) const
{
	AmberGL::CullFace(static_cast<uint8_t>(p_cullFace));
}

void AmberEditor::Rendering::Driver::SetDepthWriting(bool p_enable) const
{
	AmberGL::DepthMask(p_enable);
}

bool AmberEditor::Rendering::Driver::GetBool(int p_parameter) const
{
	bool result;
	AmberGL::GetBool(p_parameter, &result);
	return result;
}

int AmberEditor::Rendering::Driver::GetInt(int p_parameter) const
{
	int result;
	AmberGL::GetInt(p_parameter, &result);
	return result;
}

void AmberEditor::Rendering::Driver::Initialize() const
{
	AmberGL::Initialize(800, 600);
}
