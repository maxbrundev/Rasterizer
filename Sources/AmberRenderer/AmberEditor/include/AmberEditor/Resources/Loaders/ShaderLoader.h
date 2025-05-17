#pragma once

#include <string>

#include <AmberGL/SoftwareRenderer/Programs/AProgram.h>

#include "AmberEditor/Resources/Shader.h"

namespace AmberEditor::Resources::Loaders
{
	template <typename T>
	concept DerivedFromAProgram = std::is_base_of_v<AmberGL::SoftwareRenderer::Programs::AProgram, T>;

	class ShaderLoader
	{
	public:
		ShaderLoader() = delete;

		//Meh..temporary 
		template <DerivedFromAProgram T>
		static AShader* Create()
		{
			return new Shader<T>();
		}

		static bool Destroy(AShader*& p_shaderInstance);
		static bool Delete(AShader* p_shaderInstance);
	};
}
