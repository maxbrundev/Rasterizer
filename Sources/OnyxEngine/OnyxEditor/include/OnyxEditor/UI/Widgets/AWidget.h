#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace OnyxEditor::UI::Widgets
{
	class WidgetContainer;
}

namespace OnyxEditor::UI::Widgets
{
	class AWidget
	{
	public:
		AWidget();
		virtual ~AWidget() = default;

		virtual void Draw();

		void LinkTo(const AWidget& p_widget);
		void Destroy();
		bool IsDestroyed() const;
		void SetParent(WidgetContainer* p_parent);
		bool HasParent() const;

		WidgetContainer* GetParent() const;

		void AddBehavior(std::function<void()> p_behavior);
		void ExecuteBehaviors();

	protected:
		virtual void DrawImplementation() = 0;

	public:
		bool Enabled = true;
		bool LineBreak = true;

	protected:
		WidgetContainer* m_parent = nullptr;
		std::string m_widgetID = "?";
		std::vector<std::function<void()>> m_behaviors;

	private:
		static uint64_t WIDGET_ID_INCREMENT;
		bool m_destroyed = false;
	};
}