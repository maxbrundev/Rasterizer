#pragma once

#include <memory>
#include <vector>

#include "OnyxEditor/UI/Widgets/AWidget.h"

namespace OnyxEditor::UI::Widgets
{
	class WidgetContainer
	{
	public:
		WidgetContainer() = default;
		virtual ~WidgetContainer();

		void RemoveWidget(AWidget& p_widget);
		void RemoveAllWidgets();
		void CollectGarbage();
		void DrawWidgets();
		void SetReverseDrawOrder(bool p_value = true);

		template <typename T, typename... Args>
		T& CreateWidget(Args&&... p_args)
		{
			Widgets.emplace_back(std::make_unique<T>(p_args...));
			T& instance = *static_cast<T*>(Widgets.back().get());
			instance.SetParent(this);
			return instance;
		}

		std::vector<std::unique_ptr<Widgets::AWidget>>& GetWidgets();

	public:
		std::vector<std::unique_ptr<Widgets::AWidget>> Widgets;
		bool IsReversedDrawOrder = false;
	};
}