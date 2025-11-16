#pragma once

#include <string>

#include <glm/glm.hpp>

#include "OnyxEditor/Eventing/Event.h"

#include "OnyxEditor/UI/Panels/APanel.h"
#include "OnyxEditor/UI/Panels/MenuBarContainer.h"
#include "OnyxEditor/UI/Panels/PanelSettings.h"

namespace OnyxEditor::UI::Panels
{
	enum class EHorizontalAlignment
	{
		LEFT,
		CENTER,
		RIGHT
	};

	enum class EVerticalAlignment
	{
		TOP,
		MIDDLE,
		BOTTOM
	};

	class APanelWindow : public APanel, public MenuBarContainer
	{
	public:
		APanelWindow(const std::string& p_title = "", bool p_opened = true, const PanelSettings& p_panelSettings = PanelSettings{});
		virtual ~APanelWindow() override = default;

		void Update();
		void UpdateSize();
		void CopyImGuiSize();
		void UpdatePosition();
		void CopyImGuiPosition();

		void Open();
		void Close();
		
		void Focus();

		bool IsHovered() const;
		bool IsFocused() const;
		bool IsOpened() const;

		void SetOpened(bool p_value);
		void SetPosition(const glm::vec2& p_position);
		void SetSize(const glm::vec2& p_size);

		const glm::vec2& GetSize() const;

	protected:
		void DrawImplementation() override;

		glm::vec2 ComputeAlignmentOffset() const;

	public:
		std::string Title;
		PanelSettings Settings;

		glm::vec2 MinSize = glm::vec2(0.0f, 0.0f);
		glm::vec2 MaxSize = glm::vec2(0.0f, 0.0f);

		Eventing::Event<> OpenEvent;
		Eventing::Event<> CloseEvent;

	protected:
		glm::vec2 m_position;
		glm::vec2 m_size;
		glm::vec2 m_defaultPosition = glm::vec2(-1.0f, -1.0f);
		glm::vec2 m_defaultSize = glm::vec2(-1.0f, -1.0f);
		glm::vec2 m_viewportSize;

		EHorizontalAlignment m_horizontalAlignment = EHorizontalAlignment::LEFT;
		EVerticalAlignment m_verticalAlignment = EVerticalAlignment::TOP;

		bool m_firstFrame = true;
		bool m_opened;
		bool m_hovered;
		bool m_focused;
		bool m_ignoreConfigFile;
		bool m_positionChanged;
		bool m_sizeChanged;
	};
}
