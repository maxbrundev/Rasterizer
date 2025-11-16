#include "OnyxEditor/UI/Panels/DebugPanel.h"

#include <iostream>

#include <AmberGL/SoftwareRenderer/AmberGL.h>

#include "OnyxEditor/ImGUI/imgui.h"

#include "OnyxEditor/Rendering/Debug/EDebugViewMode.h"
#include "OnyxEditor/Rendering/Features/ShadowMappingRenderFeature.h"
#include "OnyxEditor/Rendering/Passes/ShadowMappingPass.h"

#include "OnyxEditor/UI/Panels/FrameDebuggerPanel.h"
#include "OnyxEditor/UI/Widgets/Button.h"
#include "OnyxEditor/UI/Widgets/ComboBox.h"
#include "OnyxEditor/UI/Widgets/Group.h"
#include "OnyxEditor/UI/Widgets/Separator.h"
#include "OnyxEditor/UI/Widgets/SliderFloat.h"
#include "OnyxEditor/UI/Widgets/Text.h"
#include "OnyxEditor/UI/Widgets/TextColored.h"

OnyxEditor::UI::Panels::DebugPanel::DebugPanel(const std::string& p_title, bool p_opened, const PanelSettings& p_panelSettings) :
APanelWindow(p_title, p_opened,p_panelSettings),
m_frameDebuggerPanel(nullptr),
m_selectedViewMode(0),
m_selectedRasterMode(0),
m_selectedAntialiasingMode(0)
{
	CreateHeaderSection();
	CreateFrameDebuggerSection();
	CreateDebugViewModesSection();
	CreateRasterizationSection();
	CreateAntialiasingSection();
	CreatePerformanceSection();
}

void OnyxEditor::UI::Panels::DebugPanel::SetFrameDebuggerPanel(FrameDebuggerPanel* p_frameDebuggerPanel)
{
	m_frameDebuggerPanel = p_frameDebuggerPanel;
}

void OnyxEditor::UI::Panels::DebugPanel::CreateHeaderSection()
{
	auto& headerText = CreateWidget<Widgets::TextColored>("SOFTWARE RENDERER");
	headerText.Color = Data::Color(66, 150, 250, 255);

	m_resolutionText = &CreateWidget<Widgets::Text>("");
}

void OnyxEditor::UI::Panels::DebugPanel::CreateFrameDebuggerSection()
{
	CreateWidget<Widgets::Text>("");

	auto& header = CreateWidget<Widgets::TextColored>("FRAME DEBUGGER");
	header.Color = Data::Color(204, 204, 204, 255);

	CreateWidget<Widgets::Separator>();

	m_frameDebuggerStatusGroup = &CreateWidget<Widgets::Group>();

	m_frameDebuggerStatusText = &m_frameDebuggerStatusGroup->CreateWidget<Widgets::TextColored>("[READY]");
	m_frameDebuggerStatusText->Color = Data::Color(153, 153, 153, 255);
	m_frameDebuggerStatusText->LineBreak = false;

	auto& captureBtn = m_frameDebuggerStatusGroup->CreateWidget<Widgets::Button>("Capture Frame");
	captureBtn.ClickedEvent += [this]() { OnCaptureFrameClick(); };

	m_capturedStepsText = &CreateWidget<Widgets::Text>("");
	m_capturedStepsText->Enabled = false;

	m_currentStepText = &CreateWidget<Widgets::Text>("");
	m_currentStepText->Enabled = false;

	m_noFrameCapturedText = &CreateWidget<Widgets::TextColored>("No frame captured yet");
	m_noFrameCapturedText->Color = Data::Color(153, 153, 153, 255);

	m_frameDebuggerDisabledText = &CreateWidget<Widgets::TextColored>("Frame Debugger Disabled");
	m_frameDebuggerDisabledText->Color = Data::Color(255, 76, 0, 255);
	m_frameDebuggerDisabledText->Enabled = false;
}

void OnyxEditor::UI::Panels::DebugPanel::CreateDebugViewModesSection()
{
	CreateWidget<Widgets::Text>("");

	auto& header = CreateWidget<Widgets::TextColored>("DEBUG VIEW MODES");
	header.Color = Data::Color(204, 204, 204, 255);

	CreateWidget<Widgets::Separator>();

	m_activeViewModeText = &CreateWidget<Widgets::TextColored>("");
	m_activeViewModeText->Color = Data::Color(76, 204, 76, 255);
	m_activeViewModeText->Enabled = false;

	auto& viewModeCombo = CreateWidget<Widgets::ComboBox>(m_selectedViewMode);
	viewModeCombo.Choices[0] = "Standard";
	viewModeCombo.Choices[1] = "Wireframe";
	viewModeCombo.Choices[2] = "World Normals";
	viewModeCombo.Choices[3] = "UV Coordinates";
	viewModeCombo.Choices[4] = "Depth Buffer";
	viewModeCombo.Choices[5] = "UV Checkerboard";
	viewModeCombo.ValueChangedEvent += [this](int p_index) { OnViewModeChanged(p_index); };

	m_lineThicknessSlider = &CreateWidget<Widgets::SliderFloat>(1.0f, 50.0f, 1.0f, "Line Thickness", "%.1f");
	m_lineThicknessSlider->Enabled = false;
	m_lineThicknessSlider->ValueChangedEvent += [this](float p_value) { OnLineThicknessChanged(p_value); };

	m_depthBufferInfoText = &CreateWidget<Widgets::Text>("Post process Depth Buffer Texture render on a NDC Unit Quad");
	m_depthBufferInfoText->Enabled = false;
}

void OnyxEditor::UI::Panels::DebugPanel::CreateRasterizationSection()
{
	CreateWidget<Widgets::Text>("");

	auto& header = CreateWidget<Widgets::TextColored>("RASTERIZATION MODE");
	header.Color = Data::Color(204, 204, 204, 255);

	CreateWidget<Widgets::Separator>();

	auto& rasterCombo = CreateWidget<Widgets::ComboBox>(m_selectedRasterMode);
	rasterCombo.Choices[0] = "FILL";
	rasterCombo.Choices[1] = "LINE";
	rasterCombo.Choices[2] = "POINT";
	rasterCombo.ValueChangedEvent += [this](int p_index) { OnRasterizationModeChanged(p_index); };
}

void OnyxEditor::UI::Panels::DebugPanel::CreateAntialiasingSection()
{
	CreateWidget<Widgets::Text>("");

	auto& header = CreateWidget<Widgets::TextColored>("ANTIALIASING");
	header.Color = Data::Color(204, 204, 204, 255);

	CreateWidget<Widgets::Separator>();

	auto& aaCombo = CreateWidget<Widgets::ComboBox>(m_selectedAntialiasingMode);
	aaCombo.Choices[0] = "NONE";
	aaCombo.Choices[1] = "MSAA";
	aaCombo.ValueChangedEvent += [this](int p_index) { OnAntialiasingModeChanged(p_index); };

	m_msaaSampleGroup = &CreateWidget<Widgets::Group>();
	m_msaaSampleGroup->Enabled = false;

	auto& btn4x = m_msaaSampleGroup->CreateWidget<Widgets::Button>("x4", glm::vec2(50.0f, 0.0f));
	btn4x.LineBreak = false;
	btn4x.ClickedEvent += [this]() { OnMSAASampleClick(4); };

	auto& btn8x = m_msaaSampleGroup->CreateWidget<Widgets::Button>("x8", glm::vec2(50.0f, 0.0f));
	btn8x.LineBreak = false;
	btn8x.ClickedEvent += [this]() { OnMSAASampleClick(8); };

	auto& btn16x = m_msaaSampleGroup->CreateWidget<Widgets::Button>("x16", glm::vec2(50.0f, 0.0f));
	btn16x.ClickedEvent += [this]() { OnMSAASampleClick(16); };
}

void OnyxEditor::UI::Panels::DebugPanel::CreatePerformanceSection()
{
	m_frameTimeText = &CreateWidget<Widgets::Text>("");
	m_fpsText = &CreateWidget<Widgets::TextColored>("");
}

void OnyxEditor::UI::Panels::DebugPanel::Initialize(OnyxEditor::Core::Context& p_context)
{
	m_context = &p_context;
}

void OnyxEditor::UI::Panels::DebugPanel::UpdateWidgets()
{
	m_resolutionText->Content = "Resolution: " + std::to_string(m_context->Window->GetSize().first) + "x" + std::to_string(m_context->Window->GetSize().second);

	UpdateFrameDebuggerState();
	UpdateDebugViewModeState();

	m_msaaSampleGroup->Enabled = m_selectedAntialiasingMode == 1;

	const float framerate = ImGui::GetIO().Framerate;
	m_frameTimeText->Content = "Frame Time: " + std::string(std::to_string(1000.0f / framerate).substr(0, 5)) + " ms";

	m_fpsText->Content = "FPS: " + std::string(std::to_string(framerate).substr(0, 5));
	m_fpsText->Color = framerate > 30.0f ? Data::Color(76, 204, 76, 255) : Data::Color(255, 153, 0, 255);
}

void OnyxEditor::UI::Panels::DebugPanel::UpdateFrameDebuggerState()
{
	auto* frameDebugger = m_context->Renderer->GetFrameDebugger();

	if (!frameDebugger)
	{
		m_frameDebuggerStatusGroup->Enabled = false;
		m_capturedStepsText->Enabled = false;
		m_currentStepText->Enabled = false;
		m_noFrameCapturedText->Enabled = false;
		m_frameDebuggerDisabledText->Enabled = true;

		return;
	}

	m_frameDebuggerStatusGroup->Enabled = true;
	m_frameDebuggerDisabledText->Enabled = false;

	if (frameDebugger->IsCapturingFrame())
	{
		m_frameDebuggerStatusText->Content = "[CAPTURING]";
		m_frameDebuggerStatusText->Color = Data::Color(255, 76, 0, 255);
	}
	else if (frameDebugger->HasCapturedSteps())
	{
		m_frameDebuggerStatusText->Content = "[CAPTURED]";
		m_frameDebuggerStatusText->Color = Data::Color(76, 204, 76, 255);
	}
	else
	{
		m_frameDebuggerStatusText->Content = "[READY]";
		m_frameDebuggerStatusText->Color = Data::Color(153, 153, 153, 255);
	}

	if (frameDebugger->HasCapturedSteps())
	{
		m_capturedStepsText->Enabled = true;
		m_capturedStepsText->Content = "Captured " + std::to_string(frameDebugger->GetStepsCount()) + " steps";

		m_currentStepText->Enabled = true;
		m_currentStepText->Content = "Current step: " + std::to_string(frameDebugger->GetSelectedStep() + 1) + " / " + std::to_string(frameDebugger->GetStepsCount());

		m_noFrameCapturedText->Enabled = false;
	}
	else
	{
		m_capturedStepsText->Enabled = false;
		m_currentStepText->Enabled = false;
		m_noFrameCapturedText->Enabled = true;
	}
}

void OnyxEditor::UI::Panels::DebugPanel::UpdateDebugViewModeState()
{
	auto currentMode = m_context->Renderer->GetDebugViewMode();

	if (currentMode != Rendering::Debug::EDebugViewMode::STANDARD)
	{
		m_activeViewModeText->Enabled = true;
		m_activeViewModeText->Content = "[ACTIVE] " + std::string(m_context->Renderer->GetViewModeName(currentMode));
	}
	else
	{
		m_activeViewModeText->Enabled = false;
	}

	m_lineThicknessSlider->Enabled = m_selectedViewMode == 1;
	m_depthBufferInfoText->Enabled = m_selectedViewMode == 4;
}

void OnyxEditor::UI::Panels::DebugPanel::OnCaptureFrameClick()
{
	auto* frameDebugger = m_context->Renderer->GetFrameDebugger();

	if (frameDebugger)
	{
		frameDebugger->StartCapture();

		if (m_frameDebuggerPanel)
		{
			if (m_frameDebuggerPanel->IsOpened())
			{
				m_frameDebuggerPanel->Close();
			}
			else
			{
				m_frameDebuggerPanel->Open();
			}
		}
	}
}

void OnyxEditor::UI::Panels::DebugPanel::OnViewModeChanged(int p_viewModeIndex)
{
	m_selectedViewMode = p_viewModeIndex;
	auto newMode = static_cast<Rendering::Debug::EDebugViewMode>(p_viewModeIndex);
	m_context->Renderer->SetDebugViewMode(newMode);
}

void OnyxEditor::UI::Panels::DebugPanel::OnRasterizationModeChanged(int p_modeIndex)
{
	m_selectedRasterMode = p_modeIndex;

	switch (p_modeIndex)
	{
	case 0:
		m_context->Driver->SetRasterizationMode(Rendering::Settings::EPolygonMode::FILL);
		break;
	case 1:
		m_context->Driver->SetRasterizationMode(Rendering::Settings::EPolygonMode::LINE);
		break;
	case 2:
		m_context->Driver->SetRasterizationMode(Rendering::Settings::EPolygonMode::POINT);
		break;
	}
}

void OnyxEditor::UI::Panels::DebugPanel::OnAntialiasingModeChanged(int p_modeIndex)
{
	m_selectedAntialiasingMode = p_modeIndex;

	switch (p_modeIndex)
	{
	case 0:
		AmberGL::Disable(AGL_MULTISAMPLE);
		break;
	case 1:
		AmberGL::Enable(AGL_MULTISAMPLE);
		AmberGL::SetSamples(4);
		break;
	}
}

void OnyxEditor::UI::Panels::DebugPanel::OnMSAASampleClick(int p_samples)
{
	AmberGL::SetSamples(p_samples);
}

void OnyxEditor::UI::Panels::DebugPanel::OnLineThicknessChanged(float p_thickness)
{
	m_lineThickness = p_thickness;
	m_context->Driver->SetRasterizationLinesWidth(p_thickness);
}
