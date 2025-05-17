#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "AmberEditor/Resources/UniformInfo.h"

namespace AmberEditor::Resources
{
	class AShader
	{
	public:
		AShader(int p_id) : programID(p_id)
		{
		}

		virtual ~AShader() = default;

		virtual void Bind()const = 0;
		virtual void Unbind()const = 0;

		virtual void QueryUniforms() = 0;

		template <typename T>
		void SetUniform(const std::string p_name, const T& p_value)
		{
			if constexpr (std::is_same_v<T, int>)
				SetUniformInt(p_name, p_value);
			else if constexpr (std::is_same_v<T, float>)
				SetUniformFloat(p_name, p_value);
			else if constexpr (std::is_same_v<T, glm::vec2>)
				SetUniformVec2(p_name, p_value);
			else if constexpr (std::is_same_v<T, glm::vec3>)
				SetUniformVec3(p_name, p_value);
			else if constexpr (std::is_same_v<T, glm::vec4>)
				SetUniformVec4(p_name, p_value);
			else if constexpr (std::is_same_v<T, glm::mat4>)
				SetUniformMat4(p_name, p_value);
		}

		const UniformInfo* GetUniformInfo(const std::string& p_name) const
		{
			for (const auto& uniform : m_uniforms)
			{
				if (uniform.Name == p_name)
				{
					return &uniform;
				}
			}
			return nullptr;
		}

		const std::vector<UniformInfo>& GetUniforms()
		{
			return m_uniforms;
		}

	protected:
		virtual void SetUniformInt(const std::string p_name, int p_value) = 0;
		virtual void SetUniformFloat(const std::string p_name, float p_value) = 0;
		virtual void SetUniformVec2(const std::string p_name, const glm::vec2& p_value) = 0;
		virtual void SetUniformVec3(const std::string p_name, const glm::vec3& p_value) = 0;
		virtual void SetUniformVec4(const std::string p_name, const glm::vec4& p_value) = 0;
		virtual void SetUniformMat4(const std::string p_name, const glm::mat4& p_value) = 0;

	public:
		const int programID;

	protected:
		std::vector<UniformInfo> m_uniforms;
	};
}
