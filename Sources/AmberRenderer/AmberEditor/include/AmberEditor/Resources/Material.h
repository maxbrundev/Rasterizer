#pragma once

#include <map>
#include <string>

#include "AmberEditor/Data/StateMask.h"
#include "AmberEditor/Resources/Shader.h"
#include "AmberEditor/Resources/Texture.h"

namespace AmberEditor::Resources
{
	class Material
	{
	public:
		Material() = default;
		~Material();

		void SetShader(AShader* p_shader);

		void Bind(Texture* p_emptyTexture = nullptr) const;
		void Unbind() const;

		void FillUniform();

		template<typename T>
		void SetUniform(const std::string& p_name, const T& p_value)
		{
			if (m_uniformsData.find(p_name) != m_uniformsData.end())
				m_uniformsData[p_name] = std::any(p_value);
		}

		template<typename T>
		const T& GetUniform(const std::string& p_name)
		{
			if (m_uniformsData.find(p_name) != m_uniformsData.end())
			{
				return std::any_cast<T>(m_uniformsData.at(p_name));
			}

			return T();
		}

		Data::StateMask GenerateStateMask() const;

		void SetBlendable(bool p_blendable);
		void SetBackFaceCulling(bool p_backFaceCulling);
		void SetFrontFaceCulling(bool p_frontFaceCulling);
		void SetDepthTest(bool p_depthTest);
		void SetDepthWriting(bool p_depthWriting);
		void SetColorWriting(bool p_colorWriting);
		void SetCastShadows(bool p_castShadows);
		void SetReceiveShadows(bool p_receiveShadows);

		bool HasShader() const;
		bool HasUniform(const std::string& p_name) const;
		bool IsBlendable() const;
		bool HasBackFaceCulling() const;
		bool HasFrontFaceCulling() const;
		bool HasDepthTest() const;
		bool HasDepthWriting() const;
		bool HasColorWriting() const;
		bool IsShadowCaster() const;
		bool IsShadowReceiver() const;

		void SetName(const std::string& p_name);
		
		AShader* GetShader() const;
		const std::string& GetName();

	private:
		AShader* m_shader = nullptr;

		std::map<std::string, std::any> m_uniformsData;

		std::string m_name;

		bool m_blendable = false;
		bool m_backFaceCulling = true;
		bool m_frontFaceCulling = false;
		bool m_depthTest = true;
		bool m_depthWriting = true;
		bool m_colorWriting = true;
		bool m_castShadows = true;
		bool m_receiveShadows = true;
	};
}
