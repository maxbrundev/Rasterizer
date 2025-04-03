#include "AmberEditor/Resources/Model.h"

AmberEditor::Resources::Model::Model(const std::string& p_filePath) : Path(p_filePath)
{
}

AmberEditor::Resources::Model::~Model()
{
	for (auto& mesh : m_meshes)
	{
		delete mesh;
		mesh = nullptr;
	}

	for (auto& material : m_materials)
	{
		delete material;
		material = nullptr;
	}
}

std::vector<AmberEditor::Resources::Mesh*>& AmberEditor::Resources::Model::GetMeshes()
{
	return m_meshes;
}

std::vector<AmberEditor::Resources::Material*>& AmberEditor::Resources::Model::GetMaterials()
{
	return m_materials;
}