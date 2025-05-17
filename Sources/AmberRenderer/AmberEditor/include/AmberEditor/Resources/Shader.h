#pragma once

#include <string>

#include <AmberGL/SoftwareRenderer/AmberGL.h>
#include <AmberGL/SoftwareRenderer/Programs/AProgram.h>

#include "AmberEditor/Resources/AShader.h"
#include "AmberEditor/Resources/Texture.h"
#include "AmberEditor/Resources/UniformInfo.h"

#include "AmberEditor/Tools/Utils/Enum.h"

namespace AmberEditor::Resources::Loaders
{
	class ShaderLoader;
}

namespace AmberEditor::Resources
{
	template<typename T>
	concept DerivedFromAProgram = std::is_base_of_v<AmberGL::SoftwareRenderer::Programs::AProgram, T>;

	template<DerivedFromAProgram T>
	class Shader : public AShader
	{
		friend class Loaders::ShaderLoader;

	public:
		Shader() : AShader(AmberGL::CreateProgram())
		{
			m_program = new T();

			AmberGL::AttachShader(programID, m_program);
			QueryUniforms();
		}

		~Shader() override
		{
			AmberGL::DeleteProgram(programID);
		}

		void QueryUniforms() override
		{
			m_uniforms.clear();

			if (!m_program)
				return;

			const auto& registeredUniforms = m_program->GetRegisteredUniforms();

			uint16_t location = 0;

			for (const auto& [name, typePair] : registeredUniforms)
			{
				const auto& [shaderDataType, defaultValue] = typePair;

				Rendering::Settings::EUniformType uniformType = GetValueEnum<Rendering::Settings::EUniformType>(shaderDataType);

				std::any defaultValueTest;

				switch (uniformType)
				{
				case Rendering::Settings::EUniformType::INT: defaultValueTest = std::make_any<int>();
					break;
				case Rendering::Settings::EUniformType::FLOAT: defaultValueTest = std::make_any<float>();
					break;
				case Rendering::Settings::EUniformType::VEC2: defaultValueTest = std::make_any<glm::vec2>();
					break;
				case Rendering::Settings::EUniformType::VEC3: defaultValueTest = std::make_any<glm::vec3>();
					break;
				case Rendering::Settings::EUniformType::VEC4: defaultValueTest = std::make_any<glm::vec4>();
					break;
				case Rendering::Settings::EUniformType::MAT2: defaultValueTest = std::make_any<glm::mat2>();
					break;
				case Rendering::Settings::EUniformType::MAT3: defaultValueTest = std::make_any<glm::mat3>();
					break;
				case Rendering::Settings::EUniformType::MAT4: defaultValueTest = std::make_any<glm::mat4>();
					break;
				case Rendering::Settings::EUniformType::SAMPLER_2D: defaultValueTest = std::make_any<Texture*>(nullptr);
					break;
				}

				m_uniforms.push_back({ uniformType, name, location++, defaultValueTest });
			}
		}

		void Bind() const override
		{
			AmberGL::UseProgram(programID);
		}
		void Unbind() const override
		{
			uint32_t programId = 0;
			AmberGL::UseProgram(programId);
		}

		void SetUniformInt(const std::string p_name, int p_value) override
		{
			m_program->SetUniform(p_name, p_value);
		}

		void SetUniformFloat(const std::string p_name, float p_value) override
		{
			m_program->SetUniform(p_name, p_value);
		}

		void SetUniformVec2(const std::string p_name, const glm::vec2& p_value) override
		{
			m_program->SetUniform(p_name, p_value);
		}

		void SetUniformVec3(const std::string p_name, const glm::vec3& p_value) override
		{
			m_program->SetUniform(p_name, p_value);
		}

		void SetUniformVec4(const std::string p_name, const glm::vec4& p_value) override
		{
			m_program->SetUniform(p_name, p_value);
		}

		void SetUniformMat4(const std::string p_name, const glm::mat4& p_value) override
		{
			m_program->SetUniform(p_name, p_value);
		}

		AmberGL::SoftwareRenderer::Programs::AProgram* GetProgram() const
		{
			return m_program;
		}

	private:
		AmberGL::SoftwareRenderer::Programs::AProgram* m_program;
	};
}
