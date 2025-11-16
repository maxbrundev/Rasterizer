#pragma once

#include <unordered_map>

#include <SDL2/SDL.h>

#include "OnyxEditor/Rendering/Debug/FrameDebugger.h"

#include "OnyxEditor/UI/Panels/APanelWindow.h"
#include "OnyxEditor/UI/Widgets/Columns.h"

namespace OnyxEditor::UI::Widgets
{
	class Button;
	class Text;
	class TextColored;
	class SliderInt;
	class Separator;
	class MenuList;
	class MenuItem;
	class ChildWindow;
	class Image;
}

namespace OnyxEditor::UI::Panels
{
	class FrameDebuggerPanel : public APanelWindow
	{
	public:
		FrameDebuggerPanel(const std::string& p_title, bool p_opened, const PanelSettings& p_windowSettings);
		virtual ~FrameDebuggerPanel() override;

		void Initialize(Rendering::Debug::FrameDebugger* p_frameDebugger, SDL_Renderer* p_sdlRenderer);
		void SetSDLRenderer(SDL_Renderer* p_sdlRenderer);

		void UpdateWidgets();

	private:
		void CreateMenuWidgets();
		void CreateContentWidgets();
		void UpdateWidgetStates();
		void RegenerateStepList();
		void UpdateStepListSelection();
		void UpdateFrameView();

		void OnStartCaptureClick();
		void OnStopCaptureClick();
		void OnClearClick();

		void OnFirstStepClick() const;
		void OnPrevStepClick() const;
		void OnNextStepClick();
		void OnLastStepClick();
		void OnSliderChanged(int value);

		SDL_Texture* GetOrCreateSDLTexture(uint32_t p_textureID);
		void ClearTextureCache();

	private:
		Rendering::Debug::FrameDebugger* m_frameDebugger;
		SDL_Renderer* m_sdlRenderer;

		std::unordered_map<uint32_t, SDL_Texture*> m_textureCache;
		uint32_t m_lastDisplayedTextureID;
		uint32_t m_lastCapturedStepCount;

		Widgets::MenuItem* m_startCaptureItem;
		Widgets::MenuItem* m_stopCaptureItem;
		

		Widgets::TextColored* m_statusText;
		Widgets::Text* m_stepCountText;
		Widgets::Separator* m_separator1;

		Widgets::TextColored* m_navHeader;
		Widgets::Button* m_firstButton;
		Widgets::Button* m_prevButton;
		Widgets::Button* m_nextButton;
		Widgets::Button* m_lastButton;
		Widgets::Text* m_stepInfoText;
		Widgets::SliderInt* m_stepSlider;
		Widgets::Separator* m_separator2;

		Widgets::Columns<2>* m_columns;

		Widgets::TextColored* m_stepListHeader;
		Widgets::Separator* m_stepListSeparator;
		Widgets::ChildWindow* m_stepListChild;

		Widgets::TextColored* m_frameViewHeader;
		Widgets::Separator* m_frameViewSeparator;
		Widgets::ChildWindow* m_frameViewChild;

		Widgets::Separator* m_separator3;
	};
}