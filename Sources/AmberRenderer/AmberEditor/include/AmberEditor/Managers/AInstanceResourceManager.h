#pragma once

#include <string>
#include <unordered_map>

namespace AmberEditor::Managers
{
	//Tempo
	template <typename T>
	class AInstanceResourceManager
	{
	public:
		AInstanceResourceManager() = default;
		virtual ~AInstanceResourceManager() = default;

		T* RegisterResource(const std::string& p_id, T* p_instance);
		void UnregisterResource(const std::string& p_id);
		T* GetResource(const std::string& p_id, bool p_createIfNotFound = true);
		bool IsResourceRegistered(const std::string& p_id);
		void UnloadResources();
		T* operator[](const std::string& p_id);
		std::unordered_map<std::string, T*>& GetResources();

	protected:
		virtual T* CreateResource(const std::string& p_id) = 0;
		virtual void DestroyResource(T* p_resource) = 0;

	private:
		std::unordered_map<std::string, T*> m_resources;
	};
}

#include "AmberEditor/Managers/AInstanceResourceManager.inl"
