#include "OnyxEditor/UI/Canvas.h"

#include "OnyxEditor/ImGUI/imgui.h"

OnyxEditor::UI::Canvas::~Canvas()
{
	RemoveAllPanels();
}

void OnyxEditor::UI::Canvas::AddPanel(Panels::APanel& p_panel)
{
	m_panels.push_back(std::ref(p_panel));
}

void OnyxEditor::UI::Canvas::RemovePanel(Panels::APanel& p_panel)
{
	const auto& predicate = [&p_panel](std::reference_wrapper<Panels::APanel>& p_item)
	{
		return &p_panel == &p_item.get();
	};

	m_panels.erase(std::remove_if(m_panels.begin(), m_panels.end(), predicate), m_panels.end());
}

void OnyxEditor::UI::Canvas::RemoveAllPanels()
{
	m_panels.clear();
}

void OnyxEditor::UI::Canvas::Draw() const
{
	if (!m_panels.empty())
	{
		for (auto& panel : m_panels)
		{
			panel.get().Draw();
		}
	}
}

void OnyxEditor::UI::Canvas::PostDraw() const
{
	ImGui::Render();
}