#pragma once

#include <map>
#include <string>

#include "AmberEditor/Resources/Shader.h"
#include "AmberEditor/Resources/Texture.h"

namespace AmberEditor::Resources
{
	class Material
	{
	public:
		Material() = default;
		~Material();

		void Bind(Texture* p_emptyTexture) const;
		void Unbind() const;

		bool HasShader() const;

		void FillUniform();

		template<typename T>
		void SetUniform(const std::string& p_key, const T& p_value)
		{
			if (m_uniformsData.find(p_key) != m_uniformsData.end())
				m_uniformsData[p_key] = std::any(p_value);
		}

		template<typename T>
		const T& GetUniform(const std::string& p_key)
		{
			if (m_uniformsData.find(p_key) != m_uniformsData.end())
			{
				return std::any_cast<T>(m_uniformsData.at(p_key));
			}

			return T();
		}

		void SetShader(AShader* p_shader);
		void SetName(const std::string& p_name);
		
		AShader* GetShader() const;
		const Texture* GetTexture() const;
		const std::string& GetName();

	private:
		AShader* m_shader = nullptr;
		Texture* m_texture = nullptr;

		std::map<std::string, std::any> m_uniformsData;

		std::string m_name;
	};
}
