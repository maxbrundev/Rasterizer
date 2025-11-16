#include "OnyxEditor/Resources/Loaders/ModelLoader.h"

#include "OnyxEditor/Managers/ShaderManager.h"
#include "OnyxEditor/Managers/TextureManager.h"
#include "OnyxEditor/Resources/Loaders/TextureLoader.h"
#include "OnyxEditor/Resources/Parsers/MaterialData.h"
#include "OnyxEditor/Resources/Shaders/DebugShader.h"
#include "OnyxEditor/Resources/Shaders/StandardPBRShader.h"
#include "OnyxEditor/Resources/Shaders/StandardShader.h"
#include "OnyxEditor/Tools/Globals/ServiceLocator.h"

OnyxEditor::Resources::Parsers::OBJParser OnyxEditor::Resources::Loaders::ModelLoader::OBJParser;
std::string OnyxEditor::Resources::Loaders::ModelLoader::FILE_TRACE;

OnyxEditor::Resources::Model* OnyxEditor::Resources::Loaders::ModelLoader::Create(const std::string& p_filePath)
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

void OnyxEditor::Resources::Loaders::ModelLoader::Reload(Model& p_model, const std::string& p_filePath)
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

bool OnyxEditor::Resources::Loaders::ModelLoader::Destroy(Model*& p_modelInstance)
{
	if (p_modelInstance)
	{
		delete p_modelInstance;
		p_modelInstance = nullptr;

		return true;
	}

	return false;
}

void OnyxEditor::Resources::Loaders::ModelLoader::GenerateModelMaterials(Model& p_model)
{
	uint8_t materialIndex = 0;
	
	auto& shaderManager = Tools::Globals::ServiceLocator::Get<Managers::ShaderManager>();
	auto* standardShader = shaderManager.GetShader<Resources::Shaders::StandardShader>("StandardShader");
	auto& textureManager = Tools::Globals::ServiceLocator::Get<Managers::TextureManager>();

	for (auto& materialData : p_model.m_materialData)
	{
		if (materialData.Name.empty())
			continue;

		Material* material = p_model.m_materials[materialIndex];

		if (material == nullptr)
		{
			material = new Material();
			material->SetName(materialData.Name);
			material->SetShader(standardShader);
			
		}

		material->SetUniform("u_AlbedoColor", materialData.DiffuseColor);
		
		if (!materialData.DiffuseTexturePath.empty())
		{
			Texture* texture = textureManager.LoadResource(materialData.DiffuseTexturePath, false);
			if (texture)
			{
				material->SetUniform("u_DiffuseMap", texture);
			}
		}

		if (!materialData.MetallicRoughnessTexturePath.empty())
		{
			Texture* texture = textureManager.LoadResource(materialData.MetallicRoughnessTexturePath, false);
			if (texture)
			{
				material->SetUniform("u_MetallicRoughnessMap", texture);
			}
		}

		if (!materialData.NormalTexturePath.empty())
		{
			Texture* texture = textureManager.LoadResource(materialData.NormalTexturePath, false);
			if (texture)
			{
				material->SetUniform("u_NormalMap", texture);
			}
		}

		if (!materialData.EmissiveTexturePath.empty())
		{
			Texture* texture = textureManager.LoadResource(materialData.EmissiveTexturePath, false);
			if (texture)
			{
				material->SetUniform("u_EmissiveMap", texture);
			}
		}

		if (!materialData.AOTexturePath.empty())
		{
			Texture* texture = textureManager.LoadResource(materialData.AOTexturePath, false);
			if (texture)
			{
				material->SetUniform("u_AOMap", texture);
			}
		}

		if (!materialData.DisplacementTexturePath.empty())
		{
			Texture* texture = textureManager.LoadResource(materialData.DisplacementTexturePath, false);
			if (texture)
			{
				material->SetUniform("u_DisplacementMap", texture);
			}
		}

		p_model.m_materials[materialIndex] = material;

		materialIndex++;
	}
}
