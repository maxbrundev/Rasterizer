#pragma once

#include <array>
#include <string>

#include "AmberEditor/Resources/Material.h"
#include "AmberEditor/Resources/Mesh.h"
#include "Parsers/MaterialData.h"

constexpr uint8_t MAX_MATERIAL_COUNT = 255;

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

		using MaterialList = std::array<Resources::Material*, MAX_MATERIAL_COUNT>;

		std::vector<Mesh*>& GetMeshes();
		const MaterialList& GetMaterials() const;

		void FillWithMaterial(Resources::Material& p_material);

	private:
		Model(const std::string& p_filePath);
		~Model();

	public:
		const std::string Path;
		std::vector<Parsers::MaterialData> m_materialData;

	private:
		MaterialList m_materials;
		std::vector<Mesh*> m_meshes;
	};
}
