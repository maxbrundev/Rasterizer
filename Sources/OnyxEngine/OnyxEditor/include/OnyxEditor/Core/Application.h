#pragma once

#include "OnyxEditor/Core/Context.h"
#include "OnyxEditor/Core/PanelsManager.h"

#include "OnyxEditor/UI/Canvas.h"

namespace OnyxEditor::Core
{
	class Application
	{
	public:
		Application();
		~Application();

		void Initialize();
		void Run();
		bool IsRunning() const;

	private:
		void InitializeUI();

	private:
		Context m_context;
		bool m_isRunning;

		UI::Canvas m_canvas;
		PanelsManager m_panelsManager;
	};
}
