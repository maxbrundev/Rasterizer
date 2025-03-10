#pragma once

#include <string>

#include "Rendering/Rasterizer/Shaders/AShader.h"

#include "Resources/Texture.h"

namespace Resources
{
	class Material
	{
	public:
		Material(const std::string& p_name = "default");
		~Material();

		void Bind(Texture* p_emptyTexture) const;

		bool HasShader() const;

		Rendering::Rasterizer::Shaders::AShader* GetShader() const;
		const Texture* GetTexture() const;

		void SetShader(Rendering::Rasterizer::Shaders::AShader* p_shader);
		void SetTexture(Texture* p_texture);

	public:
		const std::string Name;

	private:
		Rendering::Rasterizer::Shaders::AShader* m_shader;
		Texture* m_texture;
	};
}
