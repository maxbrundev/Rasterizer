#pragma once

#include <memory>
#include <vector>

#include "OnyxEditor/UI/Widgets/AWidget.h"

namespace OnyxEditor::UI::Panels
{
	class MenuBarContainer
	{
	public:
		MenuBarContainer() = default;
		virtual ~MenuBarContainer() = default;

		void DrawMenuBar();

		template <typename T, typename... Args>
		T& CreateMenuWidget(Args&&... p_args)
		{
			m_menuWidgets.emplace_back(std::make_unique<T>(p_args...));
			T& instance = *static_cast<T*>(m_menuWidgets.back().get());
			return instance;
		}

		void RemoveAllMenuWidgets();

	protected:
		std::vector<std::unique_ptr<Widgets::AWidget>> m_menuWidgets;
	};
}
