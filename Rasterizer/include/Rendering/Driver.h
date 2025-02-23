#pragma once

#include "Rendering/Settings/ECullFace.h"
#include "Rendering/Settings/ERasterizationMode.h"
#include "Rendering/Settings/ERenderingCapability.h"

namespace Rendering
{
	class Driver
	{
	public:
		Driver();
		~Driver() = default;

		void SetRasterizationMode(Settings::ERasterizationMode p_rasterizationMode);
		void SetCapability(Settings::ERenderingCapability p_capability, bool p_value);
		bool GetCapability(Settings::ERenderingCapability p_capability);
		void SetCullFace(Settings::ECullFace p_cullFace);
		void SetDepthWriting(bool p_enable);
		bool GetBool(uint8_t p_parameter);
		int GetInt(uint8_t p_parameter);

	private:
		void Initialize();
	};
}
