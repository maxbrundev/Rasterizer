#pragma once

#include <cstdint>

namespace OnyxEditor::Context::Settings
{
	struct WindowSettings
	{
		const char* title;
		
		uint16_t width;
		uint16_t height;
		
		bool fullScreen = false;
		bool resizable	= true;
	};
}
