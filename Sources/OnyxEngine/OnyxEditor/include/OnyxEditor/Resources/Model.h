#pragma once

#include <array>
#include <string>

#include "OnyxEditor/Maths/Transform.h"
#include "OnyxEditor/Resources/Material.h"
#include "OnyxEditor/Resources/Mesh.h"
#include "Parsers/MaterialData.h"

constexpr uint8_t MAX_MATERIAL_COUNT = 255;

namespace OnyxEditor::Resources::Loaders
{
	class ModelLoader;
}

namespace OnyxEditor::Resources
{
	class Model
	{
		friend class Loaders::ModelLoader;

		using MaterialList = std::array<Material*, MAX_MATERIAL_COUNT>;

	public:
		Model(const std::string& p_filePath);
		~Model();
		std::vector<Mesh*>& GetMeshes();
		const MaterialList& GetMaterials() const;

		void FillWithMaterial(Material& p_material);

	public:
		const std::string Path;
		std::vector<Parsers::MaterialData> m_materialData;

		Maths::Transform Transform;

	private:
		MaterialList m_materials;
		std::vector<Mesh*> m_meshes;
	};
}
