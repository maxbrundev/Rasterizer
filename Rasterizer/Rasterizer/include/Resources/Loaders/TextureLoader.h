#pragma once

#include <string>

#include "Resources/Texture.h"

namespace Resources::Loaders
{
	class TextureLoader
	{
	public:
		TextureLoader() = delete;

		static Texture* Create(std::string p_filePath, bool p_flipVertically);
		static bool Destroy(Texture*& p_textureInstance);

	private:
		static std::string FILE_TRACE;
	};
}
