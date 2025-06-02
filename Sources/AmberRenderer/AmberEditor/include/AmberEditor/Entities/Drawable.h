#pragma once

#include "AmberEditor/Resources/IMesh.h"
#include "AmberEditor/Resources/Material.h"

namespace AmberEditor::Entities
{
	struct Drawable
	{
		IMesh* Mesh;
		Resources::Material* Material;
		Rendering::Settings::EPrimitiveMode PrimitiveMode = Rendering::Settings::EPrimitiveMode::TRIANGLES;
	};
}
