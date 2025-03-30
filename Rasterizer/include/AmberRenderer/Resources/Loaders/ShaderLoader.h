#pragma once

#include <string>

#include "AmberRenderer/Resources/Shader.h"
#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/AProgram.h"

namespace AmberRenderer::Resources::Loaders
{
	template<typename T>
	concept DerivedFromAProgram = std::is_base_of_v<Rendering::SoftwareRenderer::Programs::AProgram, T>;

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

		static Shader* Create(const std::string& p_name, Rendering::SoftwareRenderer::Programs::AProgram* p_program);
		static bool Destroy(Shader*& p_shaderInstance);
		static bool Delete(Shader* p_shaderInstance);
	};
}
