#pragma once

#include <string>

#include <AmberGL/SoftwareRenderer/AmberGL.h>
#include <AmberGL/SoftwareRenderer/Programs/AProgram.h>

#include "OnyxEditor/Resources/AShader.h"
#include "OnyxEditor/Resources/Texture.h"
#include "OnyxEditor/Resources/UniformInfo.h"

#include "OnyxEditor/Tools/Utils/Enum.h"

namespace OnyxEditor::Resources::Loaders
{
	class ShaderLoader;
}

namespace OnyxEditor::Resources
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

			AmberGL::AttachShader(ProgramID, m_program);
			QueryUniforms();
		}

		~Shader() override
		{
			AmberGL::DeleteProgram(ProgramID);
		}

		void QueryUniforms() override
		{
			Uniforms.clear();

			if (!m_program)
				return;

			auto& registeredUniforms = m_program->GetRegisteredUniforms();
			int textureSlot = 0;
			uint16_t location = 0;

			for (auto& [name, typePair] : registeredUniforms)
			{
				auto& [shaderDataType, defaultValue] = typePair;

				Rendering::Settings::EUniformType uniformType = GetValueEnum<Rendering::Settings::EUniformType>(shaderDataType);

				if (uniformType == Rendering::Settings::EUniformType::SAMPLER_2D)
				{
					defaultValue = std::make_any<Texture*>(nullptr);
				}

				Uniforms.push_back({ uniformType, name, location++, defaultValue });
			}
		}

		void Bind() const override
		{
			AmberGL::UseProgram(ProgramID);
		}
		void Unbind() const override
		{
			uint32_t programId = 0;
			AmberGL::UseProgram(programId);
		}

		void SetUniformInt(const std::string& p_name, int p_value) override
		{
			m_program->SetUniform(p_name, p_value);
		}

		void SetUniformFloat(const std::string& p_name, float p_value) override
		{
			m_program->SetUniform(p_name, p_value);
		}

		void SetUniformVec2(const std::string& p_name, const glm::vec2& p_value) override
		{
			m_program->SetUniform(p_name, p_value);
		}

		void SetUniformVec3(const std::string& p_name, const glm::vec3& p_value) override
		{
			m_program->SetUniform(p_name, p_value);
		}

		void SetUniformVec4(const std::string& p_name, const glm::vec4& p_value) override
		{
			m_program->SetUniform(p_name, p_value);
		}

		void SetUniformMat4(const std::string& p_name, const glm::mat4& p_value) override
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
