#include "Resources/Loaders/ModelLoader.h"

Resources::Parsers::OBJParser Resources::Loaders::ModelLoader::OBJParser;
std::string Resources::Loaders::ModelLoader::FILE_TRACE;

Resources::Model* Resources::Loaders::ModelLoader::Create(const std::string& p_filePath)
{
	FILE_TRACE = p_filePath;

	Model* model = new Model(p_filePath);

	if (OBJParser.LoadOBJ(p_filePath, model->m_meshes))
	{
		return model;
	}

	delete model;

	return nullptr;
}

void Resources::Loaders::ModelLoader::Reload(Model& p_model, const std::string& p_filePath)
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

bool Resources::Loaders::ModelLoader::Destroy(Model*& p_modelInstance)
{
	if (p_modelInstance)
	{
		delete p_modelInstance;
		p_modelInstance = nullptr;

		return true;
	}

	return false;
}
