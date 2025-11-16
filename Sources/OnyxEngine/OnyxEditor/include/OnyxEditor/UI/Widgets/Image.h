#pragma once

#include "OnyxEditor/UI/Widgets/AWidget.h"
#include <glm/glm.hpp>
#include <cstdint>

namespace OnyxEditor::UI::Widgets
{
	union TextureID
	{
		uint32_t ID;
		void* Raw;
	};

	class Image : public AWidget
	{
	public:
		Image(uint32_t p_textureID, const glm::vec2& p_size);
		virtual ~Image() override = default;

	protected:
		void DrawImplementation() override;

	public:
		TextureID TextureID;
		glm::vec2 Size;
	};
}