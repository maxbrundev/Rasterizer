#pragma once

#include <glm/vec4.hpp>

#include "AmberEditor/Rendering/Settings/ECullFace.h"
#include "AmberEditor/Rendering/Settings/EPolygonMode.h"
#include "AmberEditor/Rendering/Settings/ERenderingCapability.h"

namespace AmberEditor::Rendering
{
	class Driver
	{
	public:
		Driver();
		~Driver();

		void SetClearColor(const glm::vec4& p_color) const;
		void Clear(bool p_colorBuffer, bool p_depthBuffer) const;
		void SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const;
		void SetRasterizationMode(Settings::EPolygonMode p_rasterizationMode) const;
		void SetCapability(Settings::ERenderingCapability p_capability, bool p_value) const;
		bool GetCapability(Settings::ERenderingCapability p_capability) const;
		void SetCullFace(Settings::ECullFace p_cullFace) const;
		void SetDepthWriting(bool p_enable) const;
		bool GetBool(int p_parameter) const;
		int GetInt(int p_parameter) const;

	private:
		void Initialize() const;
	};
}
