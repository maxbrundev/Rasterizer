#pragma once

#include "OnyxEditor/UI/Panels/APanel.h"

namespace OnyxEditor::UI
{
	class Canvas
	{
	public:
		Canvas() = default;
		~Canvas();

		Canvas(const Canvas& other) = delete;
		Canvas(Canvas&& other) = delete;
		Canvas& operator=(const Canvas& other) = delete;
		Canvas& operator=(Canvas&& other) = delete;

		void AddPanel(Panels::APanel& p_panel);
		void RemovePanel(Panels::APanel& p_panel);
		void RemoveAllPanels();
		void Draw() const;
		void PostDraw() const;

	private:
		std::vector<std::reference_wrapper<Panels::APanel>> m_panels;
	};
}
