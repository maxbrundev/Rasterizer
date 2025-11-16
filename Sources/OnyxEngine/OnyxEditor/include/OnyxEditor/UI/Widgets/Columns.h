#pragma once

#include <array>

#include "OnyxEditor/ImGUI/imgui.h"

#include "OnyxEditor/UI/Widgets/AWidget.h"
#include "OnyxEditor/UI/Widgets/WidgetContainer.h"

namespace OnyxEditor::UI::Widgets
{
	template <size_t _Size>
	class Columns : public AWidget, public WidgetContainer
	{
	public:
		Columns()
		{
			Widths.fill(-1.f);
		}

		virtual ~Columns() override = default;

	protected:
		virtual void DrawImplementation() override
		{
			ImGui::Columns(static_cast<int>(_Size), ("##" + m_widgetID).c_str(), false);

			int counter = 0;

			CollectGarbage();

			for (auto it = Widgets.begin(); it != Widgets.end();)
			{
				it->get()->Draw();

				++it;

				if (it != Widgets.end())
				{
					if (Widths[counter] != -1.0f) ImGui::SetColumnWidth(counter, Widths[counter]);
					ImGui::NextColumn();
				}

				counter++;

				if (counter == _Size)
				{
					counter = 0;
				}
			}
			ImGui::Columns(1);
		}

	public:
		std::array<float, _Size> Widths;
	};
}
