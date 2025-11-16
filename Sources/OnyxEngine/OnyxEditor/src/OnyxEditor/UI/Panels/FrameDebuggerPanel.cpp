#include "OnyxEditor/UI/Panels/FrameDebuggerPanel.h"

#include <iostream>

#include <AmberGL/SoftwareRenderer/AmberGL.h>

#include "OnyxEditor/ImGUI/imgui.h"

#include "OnyxEditor/UI/Tools/Converter.h"

#include "OnyxEditor/UI/Widgets/Button.h"
#include "OnyxEditor/UI/Widgets/ChildWindow.h"
#include "OnyxEditor/UI/Widgets/Columns.h"
#include "OnyxEditor/UI/Widgets/Image.h"
#include "OnyxEditor/UI/Widgets/MenuItem.h"
#include "OnyxEditor/UI/Widgets/MenuList.h"
#include "OnyxEditor/UI/Widgets/Selectable.h"
#include "OnyxEditor/UI/Widgets/Separator.h"
#include "OnyxEditor/UI/Widgets/SliderInt.h"
#include "OnyxEditor/UI/Widgets/Text.h"
#include "OnyxEditor/UI/Widgets/TextColored.h"

OnyxEditor::UI::Panels::FrameDebuggerPanel::FrameDebuggerPanel(const std::string& p_title, bool p_opened, const PanelSettings& p_windowSettings) :
APanelWindow(p_title, p_opened, p_windowSettings)
{
	CreateMenuWidgets();
	CreateContentWidgets();
}

OnyxEditor::UI::Panels::FrameDebuggerPanel::~FrameDebuggerPanel()
{
	ClearTextureCache();
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::Initialize(Rendering::Debug::FrameDebugger* p_frameDebugger, SDL_Renderer* p_sdlRenderer)
{
	m_frameDebugger = p_frameDebugger;
	m_sdlRenderer = p_sdlRenderer;
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::SetSDLRenderer(SDL_Renderer* p_sdlRenderer)
{
	m_sdlRenderer = p_sdlRenderer;
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::UpdateWidgets()
{
	if (!m_frameDebugger)
	{
		return;
	}

	uint32_t currentStepCount = m_frameDebugger->GetStepsCount();

	if (currentStepCount != m_lastCapturedStepCount)
	{
		RegenerateStepList();
		m_lastCapturedStepCount = currentStepCount;
	}

	UpdateWidgetStates();
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::CreateMenuWidgets()
{
	auto& captureMenuList = CreateMenuWidget<UI::Widgets::MenuList>("Capture");

	m_startCaptureItem = &captureMenuList.CreateWidget<UI::Widgets::MenuItem>("Start Capture");
	m_startCaptureItem->ClickedEvent += [this]() { OnStartCaptureClick(); };

	m_stopCaptureItem = &captureMenuList.CreateWidget<UI::Widgets::MenuItem>("Stop Capture");
	m_stopCaptureItem->ClickedEvent += [this]() { OnStopCaptureClick(); };

	captureMenuList.CreateWidget<UI::Widgets::Separator>();

	auto& clearMenuItem = captureMenuList.CreateWidget<UI::Widgets::MenuItem>("Clear");
	clearMenuItem.ClickedEvent += [this]() { OnClearClick(); };
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::CreateContentWidgets()
{
	m_statusText = &CreateWidget<UI::Widgets::TextColored>("");
	m_statusText->Color = Data::Color(178, 178, 178, 255);
	m_statusText->LineBreak = false;

	m_stepCountText = &CreateWidget<UI::Widgets::Text>("");

	m_separator1 = &CreateWidget<UI::Widgets::Separator>();
	m_separator1->Enabled = false;

	m_navHeader = &CreateWidget<UI::Widgets::TextColored>("FRAME NAVIGATION");
	m_navHeader->Color = Data::Color(204, 204, 204, 255);
	m_navHeader->Enabled = false;

	m_firstButton = &CreateWidget<UI::Widgets::Button>("First", glm::vec2(60.0f, 0.0f));
	m_firstButton->LineBreak = false;
	m_firstButton->Enabled = false;
	m_firstButton->ClickedEvent += [this]() { OnFirstStepClick(); };

	m_prevButton = &CreateWidget<UI::Widgets::Button>("Prev", glm::vec2(60.0f, 0.0f));
	m_prevButton->LineBreak = false;
	m_prevButton->Enabled = false;
	m_prevButton->ClickedEvent += [this]() { OnPrevStepClick(); };

	m_nextButton = &CreateWidget<UI::Widgets::Button>("Next", glm::vec2(60.0f, 0.0f));
	m_nextButton->LineBreak = false;
	m_nextButton->Enabled = false;
	m_nextButton->ClickedEvent += [this]() { OnNextStepClick(); };

	m_lastButton = &CreateWidget<UI::Widgets::Button>("Last", glm::vec2(60.0f, 0.0f));
	m_lastButton->LineBreak = false;
	m_lastButton->Enabled = false;
	m_lastButton->ClickedEvent += [this]() { OnLastStepClick(); };

	m_stepInfoText = &CreateWidget<UI::Widgets::Text>("");
	m_stepInfoText->LineBreak = false;
	m_stepInfoText->Enabled = false;

	m_stepSlider = &CreateWidget<UI::Widgets::SliderInt>(0, 0, 0);
	m_stepSlider->Enabled = false;
	m_stepSlider->width = -1.0f;
	m_stepSlider->ValueChangedEvent += [this](int p_value) { OnSliderChanged(p_value); };

	m_separator2 = &CreateWidget<UI::Widgets::Separator>();
	m_separator2->Enabled = false;

	m_columns = &CreateWidget<UI::Widgets::Columns<2>>();
	m_columns->Widths[0] = 400.0f;
	m_columns->Enabled = false;

	m_stepListHeader = &m_columns->CreateWidget<UI::Widgets::TextColored>("RENDER STEPS");
	m_stepListHeader->Color = Data::Color(204, 204, 204, 255);

	m_frameViewHeader = &m_columns->CreateWidget<UI::Widgets::TextColored>("FRAME VIEW");
	m_frameViewHeader->Color = Data::Color(204, 204, 204, 255);

	m_stepListSeparator = &m_columns->CreateWidget<UI::Widgets::Separator>();
	m_frameViewSeparator = &m_columns->CreateWidget<UI::Widgets::Separator>();
	m_frameViewSeparator->Enabled = false;

	m_stepListChild = &m_columns->CreateWidget<UI::Widgets::ChildWindow>("StepList", glm::vec2(0.0f, 0.0f), true, true);

	m_frameViewChild = &m_columns->CreateWidget<UI::Widgets::ChildWindow>("FrameView", glm::vec2(0.0f, 0.0f), true, true);

	m_separator3 = &CreateWidget<UI::Widgets::Separator>();
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::UpdateWidgetStates()
{
	if (!m_frameDebugger) 
		return;

	if (m_frameDebugger->IsCapturingFrame())
	{
		m_statusText->Content = "[CAPTURING]";
		m_statusText->Color = Data::Color(255, (uint8_t)(0.3f * 255), 0, 255);
		m_stepCountText->Content = "Steps: " + std::to_string(m_frameDebugger->GetStepsCount());
	}
	else if (m_frameDebugger->HasCapturedSteps())
	{
		m_statusText->Content = "[CAPTURED]";
		m_statusText->Color = Data::Color((uint8_t)(0.3f * 255), (uint8_t)(0.8f * 255), (uint8_t)(0.3f * 255), 255);
		m_stepCountText->Content = "Total Steps: " + std::to_string(m_frameDebugger->GetStepsCount());
	}
	else
	{
		m_statusText->Content = "[READY] No frame captured - Press F2 to start capture";
		m_statusText->Color = Data::Color((uint8_t)(0.7f * 255), (uint8_t)(0.7f * 255), (uint8_t)(0.7f * 255), 255);
		m_stepCountText->Content = "";
	}

	bool isCapturing = m_frameDebugger->IsCapturingFrame();
	m_startCaptureItem->Enabled = !isCapturing;
	m_stopCaptureItem->Enabled = isCapturing;

	bool hasCapturedSteps = m_frameDebugger->HasCapturedSteps();
	m_separator1->Enabled = hasCapturedSteps;
	m_navHeader->Enabled = hasCapturedSteps;
	m_firstButton->Enabled = hasCapturedSteps;
	m_prevButton->Enabled = hasCapturedSteps;
	m_nextButton->Enabled = hasCapturedSteps;
	m_lastButton->Enabled = hasCapturedSteps;
	m_stepInfoText->Enabled = hasCapturedSteps;
	m_stepSlider->Enabled = hasCapturedSteps;
	m_separator2->Enabled = hasCapturedSteps;
	m_columns->Enabled = hasCapturedSteps;

	if (hasCapturedSteps)
	{
		uint32_t currentStep = m_frameDebugger->GetSelectedStep();
		uint32_t totalSteps = m_frameDebugger->GetStepsCount();
		m_stepInfoText->Content = "Step " + std::to_string(currentStep + 1) + " / " + std::to_string(totalSteps);

		if (m_stepSlider->max != static_cast<int>(totalSteps - 1))
		{
			m_stepSlider->max = static_cast<int>(totalSteps - 1);
		}

		if (m_stepSlider->CurrentValue != static_cast<int>(currentStep))
		{
			m_stepSlider->CurrentValue = static_cast<int>(currentStep);
		}

		UpdateStepListSelection();
		UpdateFrameView();
	}
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::RegenerateStepList()
{
	if (!m_stepListChild) 
		return;

	m_stepListChild->RemoveAllWidgets();

	if (!m_frameDebugger || !m_frameDebugger->HasCapturedSteps()) 
		return;

	const auto& steps = m_frameDebugger->GetSteps();

	for (size_t i = 0; i < steps.size(); i++)
	{
		const auto& step = steps[i];

		std::string tagText = "[STEP]";
		Data::Color tagColor(180, 180, 180, 255);

		const std::string& src = step.Description;
		if (src.find("Clear") != std::string::npos)
		{
			tagText = "[CLEAR]";
			tagColor = Data::Color(255, 150, 50, 255);
		}
		else if (src.find("Draw") != std::string::npos)
		{
			tagText = "[DRAW]";
			tagColor = Data::Color(100, 255, 100, 255);
		}

		std::string label = std::to_string(i + 1) + ": " + step.Description;

		if (step.TriangleCount > 0)
		{
			label += " (" + std::to_string(step.TriangleCount) + " tris)";
		}

		if (!step.MaterialName.empty())
		{
			label += " - " + step.MaterialName;
		}

		auto& row = m_stepListChild->CreateWidget<UI::Widgets::Group>();

		auto& tag = row.CreateWidget<UI::Widgets::TextColored>(tagText, tagColor);
		tag.LineBreak = false;

		auto& selectable = row.CreateWidget<UI::Widgets::Selectable>(label, false);

		selectable.ClickedEvent += [this, i]()
		{
			if (m_frameDebugger)
			{
				m_frameDebugger->SelectStep(static_cast<uint32_t>(i));
			}
		};
	}
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::UpdateStepListSelection()
{
	if (!m_frameDebugger || !m_stepListChild) 
		return;

	uint32_t selectedStep = m_frameDebugger->GetSelectedStep();

	auto& widgets = m_stepListChild->GetWidgets();

	for (size_t i = 0; i < widgets.size(); i++)
	{
		auto* group = dynamic_cast<UI::Widgets::Group*>(widgets[i].get());

		if (group)
		{
			auto& groupWidgets = group->GetWidgets();

			if (groupWidgets.size() >= 2)
			{
				if (auto* selectable = dynamic_cast<UI::Widgets::Selectable*>(groupWidgets[1].get()))
				{
					selectable->Selected = i == selectedStep;
				}
			}
		}
	}
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::UpdateFrameView()
{
	if (!m_frameDebugger)
		return;

	uint32_t GLTextureID = m_frameDebugger->GetCurrentStepTextureID();

	if (GLTextureID == 0)
		return;

	SDL_Texture* sdlTexture = GetOrCreateSDLTexture(GLTextureID);

	if (!sdlTexture)
		return;

	int texWidth;
	int texHeight;

	SDL_QueryTexture(sdlTexture, nullptr, nullptr, &texWidth, &texHeight);

	m_frameViewChild->RemoveAllWidgets();

	float availableWidth = GetSize().x - 420.0f;
	float availableHeight = GetSize().y - 300.0f;

	float aspectRatio = static_cast<float>(texWidth) / static_cast<float>(texHeight);

	float displayWidth;
	float displayHeight;

	if (availableWidth / availableHeight > aspectRatio)
	{
		displayHeight = availableHeight;
		displayWidth = displayHeight * aspectRatio;
	}
	else
	{
		displayWidth = availableWidth;
		displayHeight = displayWidth / aspectRatio;
	}

	uint32_t sdlTextureID = reinterpret_cast<uintptr_t>(sdlTexture);
	auto& image = m_frameViewChild->CreateWidget<UI::Widgets::Image>(sdlTextureID, glm::vec2(displayWidth, displayHeight));

	image.TextureID.Raw = static_cast<void*>(sdlTexture);

	auto& infoPanel = m_frameViewChild->CreateWidget<UI::Widgets::ChildWindow>("InfoPanel", glm::vec2(0, 150), true);

	const auto& step = m_frameDebugger->GetSteps()[m_frameDebugger->GetSelectedStep()];

	auto& stepText = infoPanel.CreateWidget<UI::Widgets::TextColored>("Step: " + step.Description);
	stepText.Color = Data::Color(220, 220, 100, 255);

	infoPanel.CreateWidget<UI::Widgets::Text>("Triangles: " + std::to_string(step.TriangleCount));

	infoPanel.CreateWidget<UI::Widgets::Text>("Material: " + (step.MaterialName.empty() ? "None" : step.MaterialName));

	std::string textureSource = "Framebuffer";

	if (step.Description.find("Shadow") != std::string::npos)
	{
		textureSource = "Shadow Map";
	}
	else if (step.Description.find("Opaque") != std::string::npos)
	{
		textureSource = "Main Pass";
	}
	else if (step.Description.find("Clear") != std::string::npos)
	{
		textureSource = "Clear Target";
	}

	std::string textureDimensions = std::to_string(texWidth) + " x " + std::to_string(texHeight) + " " + textureSource;
	infoPanel.CreateWidget<UI::Widgets::Text>("Texture: " + textureDimensions);

	infoPanel.CreateWidget<UI::Widgets::Text>("Texture ID: " + std::to_string(step.CapturedTextureID));
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::OnStartCaptureClick()
{
	if (m_frameDebugger)
	{
		m_frameDebugger->StartCapture();
	}
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::OnStopCaptureClick()
{
	if (m_frameDebugger)
	{
		m_frameDebugger->StopCapture();
	}
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::OnClearClick()
{
	if (m_frameDebugger)
	{
		m_frameDebugger->Clear();
		ClearTextureCache();
		m_lastCapturedStepCount = 0;
		RegenerateStepList();
	}
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::OnFirstStepClick() const
{
	if (m_frameDebugger)
	{
		m_frameDebugger->FirstStep();
	}
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::OnPrevStepClick() const
{
	if (m_frameDebugger)
	{
		m_frameDebugger->PrevStep();
	}
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::OnNextStepClick()
{
	if (m_frameDebugger)
	{
		m_frameDebugger->NextStep();
	}
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::OnLastStepClick()
{
	if (m_frameDebugger)
	{
		m_frameDebugger->LastStep();
	}
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::OnSliderChanged(int value)
{
	if (m_frameDebugger)
	{
		m_frameDebugger->SelectStep(static_cast<uint32_t>(value));
	}
}

SDL_Texture* OnyxEditor::UI::Panels::FrameDebuggerPanel::GetOrCreateSDLTexture(uint32_t GLTextureID)
{
	if (!m_sdlRenderer) 
		return nullptr;

	SDL_Texture* sdlTexture = nullptr;

	auto it = m_textureCache.find(GLTextureID);

	if (it == m_textureCache.end() || m_lastDisplayedTextureID != GLTextureID)
	{
		AmberGL::ActiveTexture(0);
		AmberGL::BindTexture(AGL_TEXTURE_2D, GLTextureID);

		auto* texObj = AmberGL::GetTextureObject(0);

		if (texObj && texObj->Data8 && texObj->Width > 0 && texObj->Height > 0)
		{
			if (it == m_textureCache.end())
			{
				sdlTexture = SDL_CreateTexture(m_sdlRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, texObj->Width, texObj->Height);

				if (sdlTexture)
				{
					m_textureCache[GLTextureID] = sdlTexture;
				}
			}
			else
			{
				sdlTexture = it->second;
			}

			if (sdlTexture)
			{
				SDL_UpdateTexture(sdlTexture, nullptr, texObj->Data8, texObj->Width * 4);
				m_lastDisplayedTextureID = GLTextureID;
			}
		}

		AmberGL::BindTexture(AGL_TEXTURE_2D, 0);
		AmberGL::ActiveTexture(0);
	}
	else
	{
		sdlTexture = it->second;
	}

	return sdlTexture;
}

void OnyxEditor::UI::Panels::FrameDebuggerPanel::ClearTextureCache()
{
	for (auto& [id, texture] : m_textureCache)
	{
		if (texture)
		{
			SDL_DestroyTexture(texture);
		}
	}

	m_textureCache.clear();
	m_lastDisplayedTextureID = 0;
}