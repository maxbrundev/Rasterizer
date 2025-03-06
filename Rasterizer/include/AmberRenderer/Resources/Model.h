#pragma once

#include <string>

#include "AmberRenderer/Resources/Material.h"
#include "AmberRenderer/Resources/Mesh.h"

namespace AmberRenderer::Resources::Loaders
{
	class ModelLoader;
}

namespace AmberRenderer::Resources
{
	class Model
	{
		friend class Loaders::ModelLoader;

	public:
		std::vector<Mesh*>& GetMeshes();
		std::vector<Material*>& GetMaterials();

	private:
		Model(const std::string& p_filePath);
		~Model();

	public:
		const std::string Path;

	private:
		std::vector<Mesh*> m_meshes;
		std::vector<Material*> m_materials;
	};
}
