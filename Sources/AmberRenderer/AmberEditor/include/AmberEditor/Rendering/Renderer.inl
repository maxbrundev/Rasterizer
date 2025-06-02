#pragma once

#include <stdexcept>

#include "AmberEditor/Rendering/Renderer.h"

namespace AmberEditor::Rendering
{
	template <RenderFeature T, typename ... Args>
	T& Renderer::AddFeature(Args&&... p_args)
	{
		auto feature = std::make_unique<T>(*this, std::forward<Args>(p_args)...);
		T& ref = *feature;
		m_features[typeid(T)] = std::move(feature);
		return ref;
	}

	template <RenderFeature T>
	bool Renderer::RemoveFeature()
	{
		return m_features.erase(typeid(T)) > 0;
	}

	template <RenderFeature T>
	T& Renderer::GetFeature() const
	{
		auto it = m_features.find(typeid(T));

		if (it == m_features.end()) 
		{
			throw std::runtime_error("Feature of requested type not found");
		}
		return static_cast<T&>(*it->second);

	}

	template <RenderFeature T>
	bool Renderer::HasFeature() const
	{
		return m_features.contains(typeid(T));
	}

	template <RenderPass T, typename ... Args>
	T& Renderer::AddPass(const std::string& p_name, uint32_t p_order, Args&&... p_args)
	{
		T* pass = new T(*this, std::forward<Args>(p_args)...);
		m_passes.emplace(p_order, std::make_pair(p_name, std::unique_ptr<Passes::ARenderPass>(pass)));
		return *pass;
	}

	template <RenderPass T>
	T& Renderer::GetPass(const std::string& p_name) const
	{
		for (const auto& [_, pass] : m_passes)
		{
			if (pass.first == p_name)
			{
				return dynamic_cast<T&>(*pass.second.get());
			}
		}

		return *static_cast<T*>(nullptr);
	}
}
