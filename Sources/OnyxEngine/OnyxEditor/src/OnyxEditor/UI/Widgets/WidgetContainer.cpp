#include <ranges>

#include "OnyxEditor/UI/Widgets/WidgetContainer.h"

OnyxEditor::UI::Widgets::WidgetContainer::~WidgetContainer()
{
	RemoveAllWidgets();
}

void OnyxEditor::UI::Widgets::WidgetContainer::RemoveWidget(Widgets::AWidget& p_widget)
{
	auto found = std::find_if(Widgets.begin(), Widgets.end(), [&p_widget](std::unique_ptr<Widgets::AWidget>& p_instance)
	{
		return p_instance.get() == &p_widget;
	});

	if (found != Widgets.end())
	{
		p_widget.SetParent(nullptr);
		Widgets.erase(found);
	}
}

void OnyxEditor::UI::Widgets::WidgetContainer::RemoveAllWidgets()
{
	Widgets.clear();
}

void OnyxEditor::UI::Widgets::WidgetContainer::CollectGarbage()
{
	Widgets.erase(std::remove_if(Widgets.begin(), Widgets.end(), [](std::unique_ptr<Widgets::AWidget>& p_instance)
	{
		return p_instance && p_instance->IsDestroyed();
	}), Widgets.end());
}

void OnyxEditor::UI::Widgets::WidgetContainer::DrawWidgets()
{
	CollectGarbage();

	if (IsReversedDrawOrder)
	{
		for (const auto& Widget : std::ranges::reverse_view(Widgets))
		{
			Widget.get()->Draw();
		}
	}
	else
	{
		for (const auto& widget : Widgets) widget->Draw();
	}
}

void OnyxEditor::UI::Widgets::WidgetContainer::SetReverseDrawOrder(bool p_value)
{
	IsReversedDrawOrder = p_value;
}

std::vector<std::unique_ptr<OnyxEditor::UI::Widgets::AWidget>>& OnyxEditor::UI::Widgets::WidgetContainer::GetWidgets()
{
	return Widgets;
}
