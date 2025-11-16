#pragma once

#include "OnyxEditor/Managers/AInstanceResourceManager.h"

namespace OnyxEditor::Managers
{
	template <typename T>
	inline T* AInstanceResourceManager<T>::RegisterResource(const std::string& p_id, T* p_instance)
	{
		if (auto resource = GetResource(p_id, false); resource)
			DestroyResource(resource);

		m_resources[p_id] = p_instance;
		return p_instance;
	}

	template <typename T>
	inline void AInstanceResourceManager<T>::UnregisterResource(const std::string& p_id)
	{
		m_resources.erase(p_id);
	}

	template <typename T>
	inline T* AInstanceResourceManager<T>::GetResource(const std::string& p_id, bool p_createIfNotFound)
	{
		if (auto resource = m_resources.find(p_id); resource != m_resources.end())
		{
			return resource->second;
		}

		if (p_createIfNotFound)
		{
			auto newResource = CreateResource(p_id);

			if (newResource)
				return RegisterResource(p_id, newResource);
		}

		return nullptr;
	}

	template <typename T>
	inline bool AInstanceResourceManager<T>::IsResourceRegistered(const std::string& p_id)
	{
		return m_resources.find(p_id) != m_resources.end();
	}

	template <typename T>
	inline void AInstanceResourceManager<T>::UnloadResources()
	{
		for (auto& [key, value] : m_resources)
			DestroyResource(value);

		m_resources.clear();
	}

	template <typename T>
	inline T* AInstanceResourceManager<T>::operator[](const std::string& p_id)
	{
		return GetResource(p_id);
	}

	template <typename T>
	inline std::unordered_map<std::string, T*>& AInstanceResourceManager<T>::GetResources()
	{
		return m_resources;
	}
}
