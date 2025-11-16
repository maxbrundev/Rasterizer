#include "OnyxEditor/Rendering/Driver.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

#include "OnyxEditor/Tools/Utils/Enum.h"

OnyxEditor::Rendering::Driver::Driver(const OnyxEditor::Rendering::Settings::DriverSettings& p_driverSettings)
{
	AmberGL::Initialize(960, 540);

	m_defaultRenderState = p_driverSettings.RenderState;

	m_renderState = QueryRenderState();

	SetRenderState(m_defaultRenderState);
}

OnyxEditor::Rendering::Driver::~Driver()
{
	AmberGL::Terminate();
}

void OnyxEditor::Rendering::Driver::SetRenderState(RenderState p_state)
{
	if (p_state.PolygonMode != m_renderState.PolygonMode) SetRasterizationMode(p_state.PolygonMode);
	if (p_state.LineWidth != m_renderState.LineWidth) SetRasterizationLinesWidth(p_state.LineWidth);
	if (p_state.DepthWriting != m_renderState.DepthWriting) SetDepthWriting(p_state.DepthWriting);
	if (p_state.Culling != m_renderState.Culling)SetCapability(Settings::ERenderingCapability::CULL_FACE, p_state.Culling);
	if (p_state.DepthTest != m_renderState.DepthTest) SetCapability(Settings::ERenderingCapability::DEPTH_TEST, p_state.DepthTest);
	if (p_state.MultiSample != m_renderState.MultiSample) SetCapability(Settings::ERenderingCapability::MULTISAMPLE, p_state.MultiSample);
	if (p_state.DepthFunc != m_renderState.DepthFunc) SetDepthFunc(p_state.DepthFunc);
	if (p_state.CullFace != m_renderState.CullFace) SetCullFace(p_state.CullFace);
	if (p_state.ColorWriting != m_renderState.ColorWriting) SetColorWriting(p_state.ColorWriting);
	// Stencil state
	if (p_state.StencilTest != m_renderState.StencilTest) SetCapability(Settings::ERenderingCapability::STENCIL_TEST, p_state.StencilTest);
	if (p_state.StencilFunc != m_renderState.StencilFunc ||
		p_state.StencilRef != m_renderState.StencilRef ||
		p_state.StencilMask != m_renderState.StencilMask)
	{
		SetStencilFunc(p_state.StencilFunc, p_state.StencilRef, p_state.StencilMask);
	}
	if (p_state.StencilFail != m_renderState.StencilFail ||
		p_state.StencilPassDepthFail != m_renderState.StencilPassDepthFail ||
		p_state.StencilPassDepthPass != m_renderState.StencilPassDepthPass)
	{
		SetStencilOp(p_state.StencilFail, p_state.StencilPassDepthFail, p_state.StencilPassDepthPass);
	}
	if (p_state.StencilWriteMask != m_renderState.StencilWriteMask) SetStencilMask(p_state.StencilWriteMask);

	m_renderState = p_state;
}

OnyxEditor::Rendering::RenderState OnyxEditor::Rendering::Driver::CreateRenderState() const
{
	return m_defaultRenderState;
}

void OnyxEditor::Rendering::Driver::DrawElements(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_indexCount) const
{
	AmberGL::DrawElements(GetEnumValue<uint16_t>(p_primitiveMode), p_indexCount);
}

void OnyxEditor::Rendering::Driver::DrawArrays(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_vertexCount) const
{
	AmberGL::DrawArrays(GetEnumValue<uint16_t>(p_primitiveMode), 0, p_vertexCount);
}

void OnyxEditor::Rendering::Driver::SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const
{
	AmberGL::Viewport(p_x, p_y, p_width, p_height);
}

void OnyxEditor::Rendering::Driver::SetClearColor(float p_red, float p_green, float p_blue, float p_alpha) const
{
	AmberGL::ClearColor(p_red, p_green, p_blue, p_alpha);
}

void OnyxEditor::Rendering::Driver::Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer) const
{
	uint8_t clearMask = 0;

	if (p_colorBuffer) clearMask |= AGL_COLOR_BUFFER_BIT;
	if (p_depthBuffer) clearMask |= AGL_DEPTH_BUFFER_BIT;
	if (p_stencilBuffer) clearMask |= AGL_STENCIL_BUFFER_BIT;

	if (clearMask != 0)
	{
		AmberGL::Clear(clearMask);
	}
}

void OnyxEditor::Rendering::Driver::SetRasterizationMode(Settings::EPolygonMode p_rasterizationMode) const
{
	AmberGL::PolygonMode(GetEnumValue<uint16_t>(p_rasterizationMode));
}

void OnyxEditor::Rendering::Driver::SetRasterizationLinesWidth(float p_width) const
{
	AmberGL::LineWidth(p_width);
}

void OnyxEditor::Rendering::Driver::SetRasterizationPointSize(float p_size) const
{
	AmberGL::PointSize(p_size);
}

void OnyxEditor::Rendering::Driver::SetCapability(Settings::ERenderingCapability p_capability, bool p_value) const
{
	(p_value ? AmberGL::Enable : AmberGL::Disable)(GetEnumValue<uint8_t>(p_capability));
}

bool OnyxEditor::Rendering::Driver::GetCapability(Settings::ERenderingCapability p_capability) const
{
	return AmberGL::IsEnabled(GetEnumValue<uint8_t>(p_capability));
}

void OnyxEditor::Rendering::Driver::SetCullFace(Settings::ECullFace p_cullFace) const
{
	AmberGL::CullFace(GetEnumValue<uint16_t>(p_cullFace));
}

void OnyxEditor::Rendering::Driver::SetDepthWriting(bool p_enable) const
{
	AmberGL::DepthMask(p_enable);
}

void OnyxEditor::Rendering::Driver::SetDepthFunc(Settings::EComparisonOperand p_comparisonOperand) const
{
	AmberGL::DepthFunc(GetEnumValue<uint16_t>(p_comparisonOperand));
}

void OnyxEditor::Rendering::Driver::SetColorWriting(bool p_enable) const
{
	if (p_enable)
	{
		AmberGL::Enable(AGL_COLOR_WRITE);
	}
	else
	{
		AmberGL::Disable(AGL_COLOR_WRITE);
	}
}

bool OnyxEditor::Rendering::Driver::GetBool(uint16_t p_parameter) const
{
	bool result;
	AmberGL::GetBool(p_parameter, &result);
	return result;
}

int OnyxEditor::Rendering::Driver::GetInt(uint16_t p_parameter) const
{
	int result;
	AmberGL::GetInt(p_parameter, &result);
	return result;
}

void OnyxEditor::Rendering::Driver::SetStencilFunc(Settings::EComparisonOperand p_func, int p_ref, uint32_t p_mask) const
{
	AmberGL::StencilFunc(GetEnumValue<uint16_t>(p_func), p_ref, p_mask);
}

void OnyxEditor::Rendering::Driver::SetStencilOp(EStencilOp p_sfail, EStencilOp p_dpfail, EStencilOp p_dppass) const
{
	AmberGL::StencilOp(static_cast<uint16_t>(p_sfail), static_cast<uint16_t>(p_dpfail), static_cast<uint16_t>(p_dppass));
}

void OnyxEditor::Rendering::Driver::SetStencilMask(uint32_t p_mask) const
{
	AmberGL::StencilMask(p_mask);
}

const OnyxEditor::Rendering::RenderState& OnyxEditor::Rendering::Driver::GetRenderState()
{
	return m_renderState;
}

OnyxEditor::Rendering::RenderState OnyxEditor::Rendering::Driver::QueryRenderState() const
{
	RenderState renderState;

	renderState.DepthWriting = GetBool(AGL_DEPTH_WRITE);
	renderState.ColorWriting = GetBool(AGL_COLOR_WRITE);
	renderState.DepthTest = GetBool(AGL_DEPTH_TEST);
	renderState.MultiSample = GetBool(AGL_MULTISAMPLE);
	renderState.StencilTest = GetBool(AGL_STENCIL_TEST);

	renderState.CullFace = static_cast<Settings::ECullFace>(GetInt(AGL_CULL_FACE));
	renderState.DepthFunc = static_cast<Settings::EComparisonOperand>(GetInt(AGL_DEPTH_FUNC));
	renderState.PolygonMode = static_cast<Settings::EPolygonMode>(GetInt(AGL_POLYGON));

	return renderState;
}
