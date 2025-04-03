#pragma once

#include <any>
#include <unordered_map>

namespace AmberEditor::Tools::Globals
{
	class ServiceLocator
	{
	public:
		template<typename T>
		static void Provide(T& p_service)
		{
			m_services[typeid(T).hash_code()] = std::any(&p_service);
		}

		template<typename T>
		static T& Get()
		{
			return *std::any_cast<T*>(m_services[typeid(T).hash_code()]);
		}

	private:
		static std::unordered_map<size_t, std::any> m_services;
	};
}
