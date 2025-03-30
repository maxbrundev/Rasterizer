#pragma once

#include "AmberRenderer/Resources/Model.h"
#include "AmberRenderer/Resources/Parsers/OBJParser.h"

namespace AmberRenderer::Resources::Loaders
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
