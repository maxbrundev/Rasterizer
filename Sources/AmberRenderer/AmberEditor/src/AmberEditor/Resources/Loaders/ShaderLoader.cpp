#include "AmberEditor/Resources/Loaders/ShaderLoader.h"

#include <AmberGL/SoftwareRenderer/AmberGL.h>

AmberEditor::Resources::Shader* AmberEditor::Resources::Loaders::ShaderLoader::Create(const std::string& p_name, AmberGL::SoftwareRenderer::Programs::AProgram* p_program)
{
	uint32_t programID = AmberGL::CreateProgram();
	AmberGL::AttachShader(programID, p_program);

	return new Shader(p_name, programID, p_program);
}

bool AmberEditor::Resources::Loaders::ShaderLoader::Destroy(Shader*& p_shaderInstance)
{
	if (p_shaderInstance)
	{
		delete p_shaderInstance;
		p_shaderInstance = nullptr;

		return true;
	}

	return false;
}

bool AmberEditor::Resources::Loaders::ShaderLoader::Delete(Shader* p_shaderInstance)
{
	if (p_shaderInstance)
	{
		AmberGL::DeleteProgram(p_shaderInstance->ID);

		return true;
	}

	return false;
}
