#pragma once

#include <string>
#include <unordered_map>

#include "OnyxEditor/ImGUI/imgui.h"
#include "OnyxEditor/UI/Canvas.h"
#include "OnyxEditor/UI/UIIL/Backend.h"

namespace OnyxEditor::UI::Core
{
	class UIManager
	{
	public:
		UIManager(void* p_window, void* p_renderer);
		~UIManager();

		UIManager(const UIManager& other) = delete;
		UIManager(UIManager&& other) = delete;
		UIManager& operator=(const UIManager& other) = delete;
		UIManager& operator=(UIManager&& other) = delete;

		bool LoadFont(const std::string& p_id, const std::string& p_path, float p_fontSize);
		bool UnloadFont(const std::string& p_id);
		bool UseFont(const std::string& p_id);
		void UseDefaultFont();

		void SetCanvas(Canvas& p_canvas);
		void RemoveCanvas();

		void Render();
		void PostRender();

		void EnableEditorLayoutSave(bool p_value);
		bool IsEditorLayoutSaveEnabled() const;
		void SetEditorLayoutSaveFilename(const std::string& p_filename);
		void SetEditorLayoutAutosaveFrequency(float p_frequency);
		float GetEditorLayoutAutosaveFrequency();

		void LoadLayout(const std::string& p_fileName);
		void SaveLayout(const std::string& p_fileName);
		void SaveCurrentLayout();
		void SetLayout(const std::string& p_fileName);
		void DeleteLayout(const std::string& p_fileName);
		void RenameLayout(const std::string& p_fileName, const std::string& p_newFileName);

	private:
		void ApplyStyle();

	private:
		UIIL::Backend m_backend;
		Canvas* m_canvas;
		std::unordered_map<std::string, ImFont*> m_fonts;
		std::string m_layoutSaveFilename = "imgui.ini";
		const std::string m_defaultLayout;
		const std::string m_layoutsPath;
	};
}
