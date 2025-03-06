#include "AmberRenderer/Resources/Model.h"

AmberRenderer::Resources::Model::Model(const std::string& p_filePath) : Path(p_filePath)
{
}

AmberRenderer::Resources::Model::~Model()
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

std::vector<AmberRenderer::Resources::Mesh*>& AmberRenderer::Resources::Model::GetMeshes()
{
	return m_meshes;
}

std::vector<AmberRenderer::Resources::Material*>& AmberRenderer::Resources::Model::GetMaterials()
{
	return m_materials;
}