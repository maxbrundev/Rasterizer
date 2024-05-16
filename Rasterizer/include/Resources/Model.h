#pragma once

#include <string>

#include "Resources/Mesh.h"

namespace  Resources::Loaders
{
	class ModelLoader;
}

namespace Resources
{
	class Model
	{
		friend class Loaders::ModelLoader;

	public:
		std::vector<Mesh*>& GetMeshes();

	private:
		Model(const std::string& p_filePath);
		~Model();

	public:
		const std::string Path;

	private:
		std::vector<Mesh*> m_meshes;
	};
}
