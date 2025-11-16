#include "OnyxEditor/Managers/TextureManager.h"

#include "OnyxEditor/Resources/Loaders/TextureLoader.h"

OnyxEditor::Resources::Texture* OnyxEditor::Managers::TextureManager::CreateResource(const std::string& p_path, bool isRealPathRequired)
{
	const std::string filePath = isRealPathRequired ? GetRealPath(p_path) : p_path;

	const auto texture = Resources::Loaders::TextureLoader::Create(filePath, Rendering::Settings::ETextureFilteringMode::NEAREST, Rendering::Settings::ETextureFilteringMode::NEAREST, Rendering::Settings::EWrapMode::REPEAT, Rendering::Settings::EWrapMode::REPEAT, true, true);

	if (texture)
		*reinterpret_cast<std::string*>(reinterpret_cast<char*>(texture) + offsetof(Resources::Texture, Path)) = p_path;

	return texture;
}

void OnyxEditor::Managers::TextureManager::DestroyResource(Resources::Texture* p_resource)
{
	Resources::Loaders::TextureLoader::Destroy(p_resource);
}

void OnyxEditor::Managers::TextureManager::ReloadResource(Resources::Texture* p_resource, const std::string& p_path, bool isRealPathRequired)
{
	
}
