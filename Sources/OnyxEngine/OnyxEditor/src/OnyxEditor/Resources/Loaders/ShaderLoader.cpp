#include "OnyxEditor/Resources/Loaders/ShaderLoader.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

bool OnyxEditor::Resources::Loaders::ShaderLoader::Destroy(AShader*& p_shaderInstance)
{
	if (p_shaderInstance)
	{
		delete p_shaderInstance;
		p_shaderInstance = nullptr;
		return true;
	}

	return false;
}

bool OnyxEditor::Resources::Loaders::ShaderLoader::Delete(AShader* p_shaderInstance)
{
	if (p_shaderInstance)
	{
		AmberGL::DeleteProgram(p_shaderInstance->ProgramID);

		return true;
	}

	return false;
}
