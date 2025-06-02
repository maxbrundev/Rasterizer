#include "AmberEditor/Rendering/Driver.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

#include "AmberEditor/Tools/Utils/Enum.h"

AmberEditor::Rendering::Driver::Driver(const AmberEditor::Rendering::Settings::DriverSettings& p_driverSettings)
{
	AmberGL::Initialize(800, 600);

	m_defaultRenderState = p_driverSettings.RenderState;

	m_renderState = QueryRenderState();

	SetRenderState(m_defaultRenderState);
}

AmberEditor::Rendering::Driver::~Driver()
{
	AmberGL::Terminate();
}

void AmberEditor::Rendering::Driver::SetRenderState(RenderState p_state)
{
	if (p_state.PolygonMode != m_renderState.PolygonMode) SetRasterizationMode(p_state.PolygonMode);
	if (p_state.LineWidth != m_renderState.LineWidth) SetRasterizationLinesWidth(p_state.LineWidth);
	//if (p_state.ColorWriting.Mask != m_renderState.ColorWriting.Mask) SetColorWriting(p_state.ColorWriting.R, p_state.ColorWriting.G, p_state.ColorWriting.B, p_state.ColorWriting.A);
	if (p_state.DepthWriting != m_renderState.DepthWriting) SetDepthWriting(p_state.DepthWriting);
	//if (p_state.Blending != m_renderState.Blending)SetCapability(Settings::ERenderingCapability::BLEND, p_state.Blending);
	if (p_state.Culling != m_renderState.Culling)SetCapability(Settings::ERenderingCapability::CULL_FACE, p_state.Culling);
	if (p_state.DepthTest != m_renderState.DepthTest) SetCapability(Settings::ERenderingCapability::DEPTH_TEST, p_state.DepthTest);
	if (p_state.MultiSample != m_renderState.MultiSample) SetCapability(Settings::ERenderingCapability::MULTISAMPLE, p_state.MultiSample);
	if (p_state.DepthFunc != m_renderState.DepthFunc) SetDepthFunc(p_state.DepthFunc);
	if (p_state.CullFace != m_renderState.CullFace) SetCullFace(p_state.CullFace);

	m_renderState = p_state;
}

AmberEditor::Rendering::RenderState AmberEditor::Rendering::Driver::CreateRenderState() const
{
	return m_defaultRenderState;
}

void AmberEditor::Rendering::Driver::DrawElements(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_indexCount) const
{
	AmberGL::DrawElements(GetEnumValue<uint16_t>(p_primitiveMode), p_indexCount);
}

void AmberEditor::Rendering::Driver::DrawArrays(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_vertexCount) const
{
	AmberGL::DrawArrays(GetEnumValue<uint16_t>(p_primitiveMode), 0, p_vertexCount);
}

void AmberEditor::Rendering::Driver::SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const
{
	AmberGL::Viewport(p_x, p_y, p_width, p_height);
}

void AmberEditor::Rendering::Driver::SetClearColor(float p_red, float p_green, float p_blue, float p_alpha) const
{
	AmberGL::ClearColor(p_red, p_green, p_blue, p_alpha);
}

void AmberEditor::Rendering::Driver::Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer) const
{
	uint8_t clearMask = 0;

	if (p_colorBuffer) clearMask |= AGL_COLOR_BUFFER_BIT;
	if (p_depthBuffer) clearMask |= AGL_DEPTH_BUFFER_BIT;

	//TODO: Handle stencilBuffer.

	if (clearMask != 0)
	{
		AmberGL::Clear(clearMask);
	}
}

void AmberEditor::Rendering::Driver::SetRasterizationMode(Settings::EPolygonMode p_rasterizationMode) const
{
	AmberGL::PolygonMode(GetEnumValue<uint16_t>(p_rasterizationMode));
}

void AmberEditor::Rendering::Driver::SetRasterizationLinesWidth(float p_width) const
{
	AmberGL::LineWidth(p_width);
}

void AmberEditor::Rendering::Driver::SetRasterizationPointSize(float p_size) const
{
	AmberGL::PointSize(p_size);
}

void AmberEditor::Rendering::Driver::SetCapability(Settings::ERenderingCapability p_capability, bool p_value) const
{
	(p_value ? AmberGL::Enable : AmberGL::Disable)(GetEnumValue<uint8_t>(p_capability));
}

bool AmberEditor::Rendering::Driver::GetCapability(Settings::ERenderingCapability p_capability) const
{
	return AmberGL::IsEnabled(GetEnumValue<uint8_t>(p_capability));
}

void AmberEditor::Rendering::Driver::SetCullFace(Settings::ECullFace p_cullFace) const
{
	AmberGL::CullFace(GetEnumValue<uint16_t>(p_cullFace));
}

void AmberEditor::Rendering::Driver::SetDepthWriting(bool p_enable) const
{
	AmberGL::DepthMask(p_enable);
}

void AmberEditor::Rendering::Driver::SetDepthFunc(Settings::EComparisonOperand p_comparisonOperand) const
{
	AmberGL::DepthFunc(GetEnumValue<uint16_t>(p_comparisonOperand));
}

bool AmberEditor::Rendering::Driver::GetBool(uint16_t p_parameter) const
{
	bool result;
	AmberGL::GetBool(p_parameter, &result);
	return result;
}

int AmberEditor::Rendering::Driver::GetInt(uint16_t p_parameter) const
{
	int result;
	AmberGL::GetInt(p_parameter, &result);
	return result;
}

AmberEditor::Rendering::RenderState AmberEditor::Rendering::Driver::QueryRenderState() const
{
	RenderState renderState;

	renderState.DepthWriting = GetBool(AGL_DEPTH_WRITE);
	renderState.DepthTest = GetBool(AGL_DEPTH_TEST);
	renderState.MultiSample = GetBool(AGL_MULTISAMPLE);

	renderState.CullFace = static_cast<Settings::ECullFace>(GetInt(AGL_CULL_FACE));
	renderState.DepthFunc = static_cast<Settings::EComparisonOperand>(GetInt(AGL_DEPTH_FUNC));
	renderState.PolygonMode = static_cast<Settings::EPolygonMode>(GetInt(AGL_POLYGON));

	return renderState;
}
