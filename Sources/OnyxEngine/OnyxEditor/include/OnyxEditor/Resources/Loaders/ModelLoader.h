#pragma once

#include "OnyxEditor/Resources/Model.h"
#include "OnyxEditor/Resources/Parsers/OBJParser.h"

namespace OnyxEditor::Resources::Loaders
{
	class ModelLoader
	{
	public:
		ModelLoader() = delete;

		static Model* Create(const std::string& p_filePath);
		static void Reload(Model& p_model, const std::string& p_filePath);
		static bool Destroy(Model*& p_modelInstance);

		//TODO: remove later.
		static void GenerateModelMaterials(Model& p_model);

	private:
		static Parsers::OBJParser OBJParser;
		static std::string FILE_TRACE;
	};
}
