#pragma once

#include "OnyxEditor/Managers/AResourceManager.h"

#include "OnyxEditor/Resources/Model.h"

namespace OnyxEditor::Managers
{
	class ModelManager : public Managers::AResourceManager<Resources::Model>
	{
	public:
		ModelManager() = default;
		virtual ~ModelManager() override = default;

		virtual Resources::Model* CreateResource(const std::string& p_path, bool isRealPathRequired = true) override;
		virtual void DestroyResource(Resources::Model* p_resource) override;
		virtual void ReloadResource(Resources::Model* p_resource, const std::string& p_path, bool isRealPathRequired = true) override;
	};
}
