#include "AmberEditor/Resources/Loaders/ModelLoader.h"

#include "AmberEditor/Managers/TextureManager.h"
#include "AmberEditor/Resources/Loaders/TextureLoader.h"
#include "AmberEditor/Resources/Parsers/MaterialData.h"
#include "AmberEditor/Tools/Globals/ServiceLocator.h"

AmberEditor::Resources::Parsers::OBJParser AmberEditor::Resources::Loaders::ModelLoader::OBJParser;
std::string AmberEditor::Resources::Loaders::ModelLoader::FILE_TRACE;

AmberEditor::Resources::Model* AmberEditor::Resources::Loaders::ModelLoader::Create(const std::string& p_filePath)
{
	FILE_TRACE = p_filePath;

	Model* model = new Model(p_filePath);

	if (OBJParser.LoadOBJ(p_filePath, model->m_meshes, model->m_materialData))
	{
		return model;
	}

	delete model;

	return nullptr;
}

void AmberEditor::Resources::Loaders::ModelLoader::Reload(Model& p_model, const std::string& p_filePath)
{
	Model* newModel = Create(p_filePath);

	if (newModel)
	{
		p_model.m_meshes = newModel->m_meshes;
		newModel->m_meshes.clear();

		delete newModel;
		newModel = nullptr;
	}
}

bool AmberEditor::Resources::Loaders::ModelLoader::Destroy(Model*& p_modelInstance)
{
	if (p_modelInstance)
	{
		delete p_modelInstance;
		p_modelInstance = nullptr;

		return true;
	}

	return false;
}

void AmberEditor::Resources::Loaders::ModelLoader::GenerateModelMaterials(Model& p_model)
{
	uint8_t materialIndex = 0;

	for (auto& materialData : p_model.m_materialData)
	{
		if (materialData.Name.empty())
			continue;

		Material* material = p_model.m_materials[materialIndex];

		if (!materialData.DiffuseTexturePath.empty())
		{
			Texture* texture = Tools::Globals::ServiceLocator::Get<Managers::TextureManager>().GetResource(materialData.DiffuseTexturePath);

			if (material == nullptr)
			{
				material = new Material();
				material->SetName(materialData.Name);
			}

			material->SetUniform("u_DiffuseMap", texture);

			p_model.m_materials[materialIndex] = material;
		}
	}
}
