#include "OnyxEditor/Managers/ModelManager.h"

#include "OnyxEditor/Resources/Loaders/ModelLoader.h"

OnyxEditor::Resources::Model* OnyxEditor::Managers::ModelManager::CreateResource(const std::string& p_path, bool isRealPathRequired)
{
	const std::string filePath = isRealPathRequired ? GetRealPath(p_path) : p_path;

	const auto model = Resources::Loaders::ModelLoader::Create(filePath);

	if (model)
		*reinterpret_cast<std::string*>(reinterpret_cast<char*>(model) + offsetof(Resources::Model, Path)) = p_path; // Force the resource path to fit the given path

	return model;
}

void OnyxEditor::Managers::ModelManager::DestroyResource(Resources::Model* p_resource)
{
	Resources::Loaders::ModelLoader::Destroy(p_resource);
}

void OnyxEditor::Managers::ModelManager::ReloadResource(Resources::Model* p_resource, const std::string& p_path, bool isRealPathRequired)
{
	const std::string realPath = GetRealPath(p_path);
	Resources::Loaders::ModelLoader::Reload(*p_resource, isRealPathRequired ? realPath : p_path);
}
