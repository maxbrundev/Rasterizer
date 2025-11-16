#include "OnyxEditor/UI/Widgets/AWidget.h"

#include "OnyxEditor/ImGUI/imgui.h"

uint64_t OnyxEditor::UI::Widgets::AWidget::WIDGET_ID_INCREMENT = 0;

OnyxEditor::UI::Widgets::AWidget::AWidget()
{
	m_widgetID = "##" + std::to_string(WIDGET_ID_INCREMENT++);
}

void OnyxEditor::UI::Widgets::AWidget::Draw()
{
	if (Enabled)
	{
		DrawImplementation();
		ExecuteBehaviors();

		if (!LineBreak)
		{
			ImGui::SameLine();
		}
	}
}

void OnyxEditor::UI::Widgets::AWidget::LinkTo(const AWidget& p_widget)
{
	m_widgetID = p_widget.m_widgetID;
}

void OnyxEditor::UI::Widgets::AWidget::Destroy()
{
	m_destroyed = true;
}

bool OnyxEditor::UI::Widgets::AWidget::IsDestroyed() const
{
	return m_destroyed;
}

void OnyxEditor::UI::Widgets::AWidget::SetParent(WidgetContainer* p_parent)
{
	m_parent = p_parent;
}

bool OnyxEditor::UI::Widgets::AWidget::HasParent() const
{
	return m_parent != nullptr;
}

OnyxEditor::UI::Widgets::WidgetContainer* OnyxEditor::UI::Widgets::AWidget::GetParent() const
{
	return m_parent;
}

void OnyxEditor::UI::Widgets::AWidget::AddBehavior(std::function<void()> p_behavior)
{
	m_behaviors.push_back(p_behavior);
}

void OnyxEditor::UI::Widgets::AWidget::ExecuteBehaviors()
{
	for (auto& behavior : m_behaviors)
	{
		behavior();
	}
}
