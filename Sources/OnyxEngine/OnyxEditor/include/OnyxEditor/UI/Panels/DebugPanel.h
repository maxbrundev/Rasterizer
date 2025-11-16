#pragma once

#include <string>

#include "OnyxEditor/Core/Context.h"
#include "OnyxEditor/UI/Panels/APanelWindow.h"

#include "OnyxEditor/UI/Panels/FrameDebuggerPanel.h"

namespace OnyxEditor::UI::Widgets
{
	class Text;
	class TextColored;
	class Group;
	class Button;
	class ComboBox;
	class SliderFloat;
}

namespace OnyxEditor::UI::Panels
{
	class DebugPanel : public APanelWindow
	{
	public:
		DebugPanel(const std::string& p_title, bool p_opened, const PanelSettings& p_panelSettings);
		virtual ~DebugPanel() override = default;

		void Initialize(OnyxEditor::Core::Context& p_context);

		void UpdateWidgets();

		void SetFrameDebuggerPanel(FrameDebuggerPanel* p_frameDebuggerPanel);

	private:
		void CreateHeaderSection();
		void CreateFrameDebuggerSection();
		void CreateDebugViewModesSection();
		void CreateRasterizationSection();
		void CreateAntialiasingSection();
		void CreatePerformanceSection();

		void UpdateFrameDebuggerState();
		void UpdateDebugViewModeState();

		void OnCaptureFrameClick();
		void OnViewModeChanged(int p_viewModeIndex);
		void OnRasterizationModeChanged(int p_modeIndex);
		void OnAntialiasingModeChanged(int p_modeIndex);
		void OnMSAASampleClick(int p_samples);
		void OnLineThicknessChanged(float p_thickness);

	private:
		OnyxEditor::Core::Context* m_context;
		FrameDebuggerPanel* m_frameDebuggerPanel;

		Widgets::Text* m_resolutionText;

		Widgets::Group* m_frameDebuggerStatusGroup;
		Widgets::TextColored* m_frameDebuggerStatusText;
		Widgets::Text* m_capturedStepsText;
		Widgets::Text* m_currentStepText;
		Widgets::TextColored* m_noFrameCapturedText;
		Widgets::TextColored* m_frameDebuggerDisabledText;

		Widgets::TextColored* m_activeViewModeText;
		Widgets::Text* m_depthBufferInfoText;

		Widgets::SliderFloat* m_lineThicknessSlider;

		Widgets::Group* m_msaaSampleGroup;
		Widgets::Text* m_frameTimeText;
		Widgets::TextColored* m_fpsText;
		float m_lineThickness;
		int m_selectedViewMode;
		int m_selectedRasterMode;
		int m_selectedAntialiasingMode;
	};
}