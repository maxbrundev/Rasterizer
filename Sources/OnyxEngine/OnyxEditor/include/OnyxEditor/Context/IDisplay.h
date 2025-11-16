#pragma once

#include <utility>
#include <SDL2/SDL_render.h>
namespace OnyxEditor::Context
{
	class IDisplay
	{
	public:
		virtual ~IDisplay() = default;

		virtual void Clear() const = 0;
		virtual void Present() const = 0;
		virtual void Resize(uint16_t p_width, uint16_t p_height) = 0;
		virtual void UpdateDisplayTexture(const void* p_data, uint32_t  p_rowSize) const = 0;
		virtual std::pair<uint16_t, uint16_t> GetSize() const = 0;

		virtual SDL_Texture* GetTexture() = 0;
	};
}
