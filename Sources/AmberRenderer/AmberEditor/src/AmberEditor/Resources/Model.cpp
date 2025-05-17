#include "AmberEditor/Resources/Model.h"

AmberEditor::Resources::Model::Model(const std::string& p_filePath) : Path(p_filePath)
{
	m_materials.fill(nullptr);
}

AmberEditor::Resources::Model::~Model()
{
	for (auto& mesh : m_meshes)
	{
		delete mesh;
		mesh = nullptr;
	}
}

std::vector<AmberEditor::Resources::Mesh*>& AmberEditor::Resources::Model::GetMeshes()
{
	return m_meshes;
}

const AmberEditor::Resources::Model::MaterialList& AmberEditor::Resources::Model::GetMaterials() const
{
	return m_materials;
}

void AmberEditor::Resources::Model::FillWithMaterial(Resources::Material& p_material)
{
	for (uint8_t i = 0; i < m_materials.size(); i++)
		m_materials[i] = &p_material;
}
