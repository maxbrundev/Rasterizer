#pragma once

#include "OnyxEditor/Managers/AResourceManager.h"

#include "OnyxEditor/Resources/Texture.h"

namespace OnyxEditor::Managers
{
	class TextureManager : public Managers::AResourceManager<Resources::Texture>
	{
	public:
		TextureManager() = default;
		virtual ~TextureManager() override = default;

		virtual Resources::Texture* CreateResource(const std::string& p_path, bool isRealPathRequired = true) override;
		virtual void DestroyResource(Resources::Texture* p_resource) override;
		virtual void ReloadResource(Resources::Texture* p_resource, const std::string& p_path, bool isRealPathRequired = true) override;
	};
}
