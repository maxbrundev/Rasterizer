#include "OnyxEditor/Managers/ShaderManager.h"

#include <stdexcept>

#include "OnyxEditor/Resources/Loaders/ShaderLoader.h"

void OnyxEditor::Managers::ShaderManager::DestroyResource(Resources::AShader* p_resource)
{
	Resources::Loaders::ShaderLoader::Destroy(p_resource);
}

OnyxEditor::Resources::AShader* OnyxEditor::Managers::ShaderManager::CreateResource(const std::string& p_identifier)
{
	throw std::runtime_error("ShaderManager::CreateResource is not supported. Use CreateShader<ProgramType>() instead.");
}
