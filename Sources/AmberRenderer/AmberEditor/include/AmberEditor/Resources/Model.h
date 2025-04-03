#pragma once

#include <string>

#include "AmberEditor/Resources/Material.h"
#include "AmberEditor/Resources/Mesh.h"

namespace AmberEditor::Resources::Loaders
{
	class ModelLoader;
}

namespace AmberEditor::Resources
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
