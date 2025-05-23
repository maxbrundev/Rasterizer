#pragma once

#include <functional>
#include <unordered_map>

namespace AmberEditor::Eventing
{
	template<class... ArgTypes>
	class Event
	{
	private:
		std::unordered_map<uint64_t, std::function<void(ArgTypes...)>>	m_callbacks;
		uint64_t														m_availableListenerID = 0;

	public:
		uint64_t AddListener(std::function<void(ArgTypes...)> p_callback);
		uint64_t operator+=(std::function<void(ArgTypes...)> p_callback);

		bool RemoveListener(uint64_t p_listenerID);
		bool operator-=(uint64_t p_listenerID);

		void RemoveAllListeners();

		uint64_t GetListenerCount();

		void Invoke(ArgTypes... p_args);
	};

	template <typename Func, typename Instance>
	auto QuickBind(Func function, Instance* instance)
	{
		return [=](auto&&... args)
		{
			return (instance->*function)(std::forward<decltype(args)>(args)...);
		};
	}
}

#include "AmberEditor/Eventing/Event.inl"