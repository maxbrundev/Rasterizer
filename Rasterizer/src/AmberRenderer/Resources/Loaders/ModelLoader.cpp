#include "AmberRenderer/Resources/Loaders/ModelLoader.h"

#include "AmberRenderer/Resources/Loaders/TextureLoader.h"
#include "AmberRenderer/Resources/Parsers/MaterialData.h"

AmberRenderer::Resources::Parsers::OBJParser AmberRenderer::Resources::Loaders::ModelLoader::OBJParser;
std::string AmberRenderer::Resources::Loaders::ModelLoader::FILE_TRACE;

//TODO: Implement Resource Management classes to be RAII-compliant with resources Models / Textures / Materials.
AmberRenderer::Resources::Model* AmberRenderer::Resources::Loaders::ModelLoader::Create(const std::string& p_filePath)
{
	FILE_TRACE = p_filePath;

	Model* model = new Model(p_filePath);

	std::vector<Parsers::MaterialData> materialsData;

	if (OBJParser.LoadOBJ(p_filePath, model->m_meshes, materialsData))
	{
		for (const Parsers::MaterialData& materialData : materialsData)
		{
			Texture* texture = nullptr;

			if (!materialData.DiffuseTexturePath.empty())
			{
				texture = TextureLoader::Create(materialData.DiffuseTexturePath, true, Resources::Settings::ETextureFilteringMode::NEAREST, Resources::Settings::ETextureWrapMode::REPEAT, false);
			}

			Material* material = new Material(materialData.Name);
			material->SetTexture(texture);
			model->m_materials.push_back(material);
		}
		return model;
		
	}

	delete model;

	return nullptr;
}

void AmberRenderer::Resources::Loaders::ModelLoader::Reload(Model& p_model, const std::string& p_filePath)
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

bool AmberRenderer::Resources::Loaders::ModelLoader::Destroy(Model*& p_modelInstance)
{
	if (p_modelInstance)
	{
		delete p_modelInstance;
		p_modelInstance = nullptr;

		return true;
	}

	return false;
}
