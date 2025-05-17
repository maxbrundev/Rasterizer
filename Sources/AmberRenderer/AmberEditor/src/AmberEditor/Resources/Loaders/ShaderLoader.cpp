#include "AmberEditor/Resources/Loaders/ShaderLoader.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

bool AmberEditor::Resources::Loaders::ShaderLoader::Destroy(AShader*& p_shaderInstance)
{
	if (p_shaderInstance)
	{
		delete p_shaderInstance;
		p_shaderInstance = nullptr;
		return true;
	}

	return false;
}

bool AmberEditor::Resources::Loaders::ShaderLoader::Delete(AShader* p_shaderInstance)
{
	if (p_shaderInstance)
	{
		AmberGL::DeleteProgram(p_shaderInstance->programID);

		return true;
	}

	return false;
}
