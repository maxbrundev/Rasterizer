#pragma once

#include <string>

#include "AmberEditor/Resources/Shader.h"
#include "AmberGL/SoftwareRenderer/Programs/AProgram.h"

namespace AmberEditor::Resources::Loaders
{
	template<typename T>
	concept DerivedFromAProgram = std::is_base_of_v<AmberGL::SoftwareRenderer::Programs::AProgram, T>;

	class ShaderLoader
	{
	public:
		ShaderLoader() = delete;

		template<DerivedFromAProgram T>
		static Shader* Create(const std::string& p_name)
		{
			T* program = new T();

			return Create(p_name, program);
		}

		static Shader* Create(const std::string& p_name, AmberGL::SoftwareRenderer::Programs::AProgram* p_program);
		static bool Destroy(Shader*& p_shaderInstance);
		static bool Delete(Shader* p_shaderInstance);
	};
}
