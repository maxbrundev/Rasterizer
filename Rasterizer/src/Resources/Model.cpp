#include "Resources/Model.h"

Resources::Model::Model(const std::string& p_filePath) : Path(p_filePath)
{
}

Resources::Model::~Model()
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

std::vector<Resources::Mesh*>& Resources::Model::GetMeshes()
{
	return m_meshes;
}

std::vector<Resources::Material*>& Resources::Model::GetMaterials()
{
	return m_materials;
}