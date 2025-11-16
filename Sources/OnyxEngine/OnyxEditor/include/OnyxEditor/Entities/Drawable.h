#pragma once

#include "OnyxEditor/Maths/Transform.h"
#include "OnyxEditor/Resources/IMesh.h"
#include "OnyxEditor/Resources/Material.h"

namespace OnyxEditor::Entities
{
	struct Drawable
	{
		IMesh* Mesh;
		Resources::Material* Material;
		Rendering::Settings::EPrimitiveMode PrimitiveMode = Rendering::Settings::EPrimitiveMode::TRIANGLES;

		glm::mat4 ModelMatrix;
	};
}
