#pragma once

#include "OnyxEditor/UI/Canvas.h"

#include "OnyxEditor/UI/Panels/APanel.h"
#include "OnyxEditor/UI/Panels/APanelWindow.h"

namespace OnyxEditor::Core
{
	class PanelsManager
	{
	public:
		PanelsManager(OnyxEditor::UI::Canvas& p_canvas);

		template<typename T, typename... Args>
		T& CreatePanel(const std::string& p_id, Args&&... p_args)
		{
			if constexpr (std::is_base_of<OnyxEditor::UI::Panels::APanelWindow, T>::value)
			{
				m_panels.emplace(p_id, std::make_unique<T>(p_id, std::forward<Args>(p_args)...));
			}
			else
			{
				m_panels.emplace(p_id, std::make_unique<T>(std::forward<Args>(p_args)...));
			}

			m_canvas.AddPanel(*m_panels[p_id]);

			return *static_cast<T*>(m_panels[p_id].get());
		}

		template<typename T>
		T& GetPanelAs(const std::string& p_id)
		{
			return *static_cast<T*>(m_panels[p_id].get());
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<OnyxEditor::UI::Panels::APanel>> m_panels;
		OnyxEditor::UI::Canvas& m_canvas;
	};
}
