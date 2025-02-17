#pragma once

#include <string>
#include "Rendering/AShader.h"
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

		Rendering::AShader* GetShader() const;
		const Texture* GetTexture() const;

		void SetShader(Rendering::AShader* p_shader);
		void SetTexture(Texture* p_texture);

	public:
		const std::string Name;

	private:
		Rendering::AShader* m_shader;
		Texture* m_texture;
	};
}
