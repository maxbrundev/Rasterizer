#include "OnyxEditor/UI/Panels/APanelWindow.h"

#include "OnyxEditor/ImGUI/imgui.h"
#include "OnyxEditor/UI/Tools/Converter.h"

OnyxEditor::UI::Panels::APanelWindow::APanelWindow(const std::string& p_title, bool p_opened, const PanelSettings& p_panelSettings) :
Title(p_title),
Settings(p_panelSettings),
m_opened(p_opened),
m_hovered(false),
m_focused(false),
m_defaultPosition(-1.0f, -1.0f)
{
}

void OnyxEditor::UI::Panels::APanelWindow::Update()
{
	if (!m_firstFrame)
	{
		if (!Settings.AutoSize)
		{
			UpdateSize();
		}

		CopyImGuiSize();

		UpdatePosition();

		CopyImGuiPosition();
	}

	m_firstFrame = false;
}

void OnyxEditor::UI::Panels::APanelWindow::UpdateSize()
{
	if (m_sizeChanged)
	{
		ImGui::SetWindowSize(Title.c_str(), ImVec2(m_size.x, m_size.y), ImGuiCond_Always);
		m_sizeChanged = false;
	}
}

void OnyxEditor::UI::Panels::APanelWindow::CopyImGuiSize()
{
	m_size = glm::vec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
}

void OnyxEditor::UI::Panels::APanelWindow::UpdatePosition()
{
	if (m_firstFrame && m_defaultPosition.x >= 0.0f && m_defaultPosition.y >= 0.0f)
	{
		glm::vec2 alignedPosition = m_defaultPosition + ComputeAlignmentOffset();
		ImGuiCond imguiCond = m_ignoreConfigFile ? ImGuiCond_Once : ImGuiCond_FirstUseEver;
		ImGui::SetNextWindowPos(ImVec2(alignedPosition.x, alignedPosition.y), imguiCond);
	}

	if (m_positionChanged)
	{
		glm::vec2 alignedPosition = m_position + ComputeAlignmentOffset();
		ImGui::SetWindowPos(Title.c_str(), ImVec2(alignedPosition.x, alignedPosition.y), ImGuiCond_Always);
		m_positionChanged = false;
	}
}

void OnyxEditor::UI::Panels::APanelWindow::CopyImGuiPosition()
{
	m_position = Tools::Converter::ToVec2(ImGui::GetWindowPos());
}

void OnyxEditor::UI::Panels::APanelWindow::Open()
{
	if (!m_opened)
	{
		m_opened = true;
		OpenEvent.Invoke();
	}
}

void OnyxEditor::UI::Panels::APanelWindow::Close()
{
	if (m_opened)
	{
		m_opened = false;
		CloseEvent.Invoke();
	}
}

void OnyxEditor::UI::Panels::APanelWindow::Focus()
{
	ImGui::SetWindowFocus(Title.c_str());
}

bool OnyxEditor::UI::Panels::APanelWindow::IsHovered() const
{
	return m_hovered;
}

bool OnyxEditor::UI::Panels::APanelWindow::IsFocused() const
{
	return m_focused;
}

bool OnyxEditor::UI::Panels::APanelWindow::IsOpened() const
{
	return m_opened;
}

void OnyxEditor::UI::Panels::APanelWindow::SetOpened(bool p_value)
{
	if (p_value != m_opened)
	{
		m_opened = p_value;

		if (m_opened)
		{
			OpenEvent.Invoke();
		}
		else
		{
			CloseEvent.Invoke();
		}
	}
}

void OnyxEditor::UI::Panels::APanelWindow::SetPosition(const glm::vec2& p_position)
{
	m_position = p_position;
	m_positionChanged = true;
}

void OnyxEditor::UI::Panels::APanelWindow::SetSize(const glm::vec2& p_size)
{
	m_size = p_size;
	m_sizeChanged = true;
}

const glm::vec2& OnyxEditor::UI::Panels::APanelWindow::GetSize() const
{
	return m_size;
}

void OnyxEditor::UI::Panels::APanelWindow::DrawImplementation()
{
	if (m_opened)
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
		if (Settings.MenuBar) windowFlags |= ImGuiWindowFlags_MenuBar;
		if (!Settings.Resizable) windowFlags |= ImGuiWindowFlags_NoResize;
		if (!Settings.Movable) windowFlags |= ImGuiWindowFlags_NoMove;
		if (Settings.HideBackground) windowFlags |= ImGuiWindowFlags_NoBackground;
		if (Settings.ForceHorizontalScrollbar) windowFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
		if (Settings.ForceVerticalScrollbar) windowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
		if (Settings.AllowHorizontalScrollbar) windowFlags |= ImGuiWindowFlags_HorizontalScrollbar;
		if (!Settings.BringToFrontOnFocus) windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		if (!Settings.Collapsable) windowFlags |= ImGuiWindowFlags_NoCollapse;
		if (!Settings.AllowInputs) windowFlags |= ImGuiWindowFlags_NoInputs;
		if (!Settings.Scrollable) windowFlags |= ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
		if (!Settings.TitleBar) windowFlags |= ImGuiWindowFlags_NoTitleBar;
		if (Settings.AutoSize) windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;

		if (MinSize.x > 0.0f || MinSize.y > 0.0f || MaxSize.x > 0.0f || MaxSize.y > 0.0f)
		{
			ImGui::SetNextWindowSizeConstraints(ImVec2(MinSize.x, MinSize.y), ImVec2(MaxSize.x > 0.0f ? MaxSize.x : FLT_MAX, MaxSize.y > 0.0f ? MaxSize.y : FLT_MAX));
		}

		bool* closablePtr = Settings.Closable ? &m_opened : nullptr;

		if (ImGui::Begin(Title.c_str(), closablePtr, windowFlags))
		{
			m_hovered = ImGui::IsWindowHovered();
			m_focused = ImGui::IsWindowFocused();

			Update();

			if (Settings.MenuBar && ImGui::BeginMenuBar())
			{
				DrawMenuBar();
				ImGui::EndMenuBar();
			}

			DrawWidgets();
		}
		ImGui::End();

		if (closablePtr && !m_opened)
		{
			CloseEvent.Invoke();
		}
	}
}

glm::vec2 OnyxEditor::UI::Panels::APanelWindow::ComputeAlignmentOffset() const
{
	glm::vec2 offset(0.0f);

	switch (m_horizontalAlignment)
	{
	case EHorizontalAlignment::CENTER:
		offset.x -= m_size.x * 0.5f;
		break;
	case EHorizontalAlignment::RIGHT:
		offset.x -= m_size.x;
		break;
	default: break;
	}

	switch (m_verticalAlignment)
	{
	case EVerticalAlignment::MIDDLE:
		offset.y -= m_size.y * 0.5f;
		break;
	case EVerticalAlignment::BOTTOM:
		offset.y -= m_size.y;
		break;
	default: break;
	}

	return offset;
}
