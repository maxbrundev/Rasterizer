#include "AmberEditor/Managers/ShaderManager.h"

#include <stdexcept>

#include "AmberEditor/Resources/Loaders/ShaderLoader.h"

void AmberEditor::Managers::ShaderManager::DestroyResource(Resources::AShader* p_resource)
{
	Resources::Loaders::ShaderLoader::Destroy(p_resource);
}

AmberEditor::Resources::AShader* AmberEditor::Managers::ShaderManager::CreateResource(const std::string& p_identifier)
{
	throw std::runtime_error("ShaderManager::CreateResource is not supported. Use CreateShader<ProgramType>() instead.");
}
