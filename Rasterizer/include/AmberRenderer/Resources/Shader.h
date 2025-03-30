#pragma once

#include <string>

#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/AProgram.h"

namespace AmberRenderer::Resources::Loaders
{
	class ShaderLoader;
}

namespace AmberRenderer::Resources
{
	class Shader
	{
		friend class Loaders::ShaderLoader;

	public:
		Shader(const std::string& p_name, uint32_t p_id, Rendering::SoftwareRenderer::Programs::AProgram* p_program);
		~Shader() = default;

		void Bind() const;
		void Unbind() const;

		Rendering::SoftwareRenderer::Programs::AProgram* GetProgram() const;

		template <typename T>
		void SetUniform(const std::string_view p_name, const T& p_value) const
		{
			if (m_program != nullptr)
			{
				m_program->SetUniform(p_name, p_value);
			}
		}

	public:
		const uint32_t ID;
		const std::string Name;

	private:
		Rendering::SoftwareRenderer::Programs::AProgram* m_program;
	};
}
