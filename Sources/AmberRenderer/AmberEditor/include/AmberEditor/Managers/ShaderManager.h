#pragma once

#include "AInstanceResourceManager.h"
#include "AmberEditor/Resources/Loaders/ShaderLoader.h"

#include "AmberEditor/Resources/Shader.h"

namespace AmberEditor::Managers
{
	class ShaderManager : public Managers::AInstanceResourceManager<Resources::AShader>
	{
	public:
		ShaderManager() = default;
		virtual ~ShaderManager() override = default;

		template <Resources::DerivedFromAProgram T>
		Resources::AShader* GetShader(const std::string& p_id)
		{
			if (auto shader = GetResource(p_id, false))
				return shader;

			return CreateShaderInternal<T>(p_id);
		}

		virtual void DestroyResource(Resources::AShader* p_resource) override;

	private:
		template <Resources::DerivedFromAProgram T>
		Resources::AShader* CreateShaderInternal(const std::string& p_id)
		{
			auto shader = Resources::Loaders::ShaderLoader::Create<T>();

			return RegisterResource(p_id, shader);
		}

		virtual Resources::AShader* CreateResource(const std::string& p_identifier) override;
	};
}
