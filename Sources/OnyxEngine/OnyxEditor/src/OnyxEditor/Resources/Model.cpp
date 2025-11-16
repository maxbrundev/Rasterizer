#include "OnyxEditor/Resources/Model.h"

OnyxEditor::Resources::Model::Model(const std::string& p_filePath) : Path(p_filePath)
{
	m_materials.fill(nullptr);
}

OnyxEditor::Resources::Model::~Model()
{
	for (auto& mesh : m_meshes)
	{
		delete mesh;
		mesh = nullptr;
	}

	m_materialData.clear();

	m_materials.fill(nullptr);
}

std::vector<OnyxEditor::Resources::Mesh*>& OnyxEditor::Resources::Model::GetMeshes()
{
	return m_meshes;
}

const OnyxEditor::Resources::Model::MaterialList& OnyxEditor::Resources::Model::GetMaterials() const
{
	return m_materials;
}

void OnyxEditor::Resources::Model::FillWithMaterial(Material& p_material)
{
	for (uint8_t i = 0; i < m_materials.size(); i++)
	{
		m_materials[i] = &p_material;
	}
}
