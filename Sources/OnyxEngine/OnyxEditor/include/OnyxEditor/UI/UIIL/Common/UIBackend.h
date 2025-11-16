#pragma once

#include <string>

#include "OnyxEditor/UI/Settings/EUIBackend.h"

namespace OnyxEditor::UI::UIIL
{
	template <OnyxEditor::UI::Settings::EUIBackend Backend, class Context>
	class UIBackend final
	{
	public:
		bool Initialize(void* p_window, void* p_context = nullptr);

		void Shutdown();

		void NewFrame();

		void RenderDrawData();

		std::string GetBackendName() const;

	private:
		Context m_context;
	};
}
