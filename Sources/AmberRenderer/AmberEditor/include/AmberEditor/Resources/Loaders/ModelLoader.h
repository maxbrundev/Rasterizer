#pragma once

#include "AmberEditor/Resources/Model.h"
#include "AmberEditor/Resources/Parsers/OBJParser.h"

namespace AmberEditor::Resources::Loaders
{
	class ModelLoader
	{
	public:
		ModelLoader() = delete;

		static Model* Create(const std::string& p_filePath);
		static void Reload(Model& p_model, const std::string& p_filePath);
		static bool Destroy(Model*& p_modelInstance);

	private:
		static Parsers::OBJParser OBJParser;
		static std::string FILE_TRACE;
	};
}
