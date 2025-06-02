#pragma once

#include "AmberEditor/Rendering/Settings/EComparisonOperand.h"
#include "AmberEditor/Rendering/Settings/ECullFace.h"
#include "AmberEditor/Rendering/Settings/EPolygonMode.h"
#include "AmberEditor/Rendering/Settings/EPrimitiveMode.h"
#include "AmberEditor/Rendering/Settings/ERenderingCapability.h"
#include "Settings/DriverSettings.h"

namespace AmberEditor::Rendering
{
	class Driver
	{
	public:
		Driver(const AmberEditor::Rendering::Settings::DriverSettings& p_driverSettings);
		~Driver();

		void SetRenderState(RenderState p_state);
		RenderState CreateRenderState() const;
		void DrawElements(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_indexCount) const;
		void DrawArrays(Settings::EPrimitiveMode p_primitiveMode, uint32_t p_vertexCount) const;
		void SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const;
		void SetClearColor(float p_red, float p_green, float p_blue, float p_alpha) const;
		void Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer) const;
		void SetRasterizationMode(Settings::EPolygonMode p_rasterizationMode) const;
		void SetRasterizationLinesWidth(float p_width) const;
		void SetRasterizationPointSize(float p_size) const;
		void SetCapability(Settings::ERenderingCapability p_capability, bool p_value) const;
		bool GetCapability(Settings::ERenderingCapability p_capability) const;
		void SetCullFace(Settings::ECullFace p_cullFace) const;
		void SetDepthWriting(bool p_enable) const;
		void SetDepthFunc(Settings::EComparisonOperand p_comparisonOperand) const;
		bool GetBool(uint16_t p_parameter) const;
		int GetInt(uint16_t p_parameter) const;

	private:
		RenderState QueryRenderState() const;

	private:
		RenderState m_defaultRenderState;
		RenderState m_renderState;
	};
}
