#pragma once

#include <functional>


namespace OnyxEditor::Maths
{
	class TransformNotifier
	{
	public:
		enum class ENotification
		{
			TRANSFORM_CHANGED,
			TRANSFORM_DESTROYED
		};

		using NotificationHandler = std::function<void(ENotification)>;
		using NotificationHandlerID = uint64_t;

		NotificationHandlerID AddNotificationHandler(NotificationHandler p_notificationHandler);

		void NotifyChildren(ENotification p_notification);

		bool RemoveNotificationHandler(const NotificationHandlerID& p_notificationHandlerID);

	private:
		std::unordered_map<NotificationHandlerID, NotificationHandler> m_notificationHandlers;
		NotificationHandlerID m_availableHandlerID = 0;
	};
}