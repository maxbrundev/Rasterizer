#pragma once

#include <cstdint>

#include "OnyxEditor/Context/IDisplay.h"
#include "OnyxEditor/Context/SDLDriver.h"

namespace OnyxEditor::Context
{
	class SDLDisplay : public IDisplay
	{
	public:
		SDLDisplay(Context::SDLDriver& p_driver, uint16_t  p_width, uint16_t  p_height);
		~SDLDisplay() override;

		void Clear() const override;
		void Present() const override;
		void UpdateDisplayTexture(const void* p_data, uint32_t  p_rowSize) const override;
		void Resize(uint16_t p_width, uint16_t p_height) override;
		std::pair<uint16_t, uint16_t> GetSize() const override;
		SDL_Texture* GetTexture() override { return m_displayTexture; }

	private:
		Context::SDLDriver& m_driver;
		SDL_Texture* m_displayTexture;
		std::pair<uint16_t, uint16_t> m_size;
	};
}
