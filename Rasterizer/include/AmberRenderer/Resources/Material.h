#pragma once

#include <string>

#include "AmberRenderer/Resources/Shader.h"
#include "AmberRenderer/Resources/Texture.h"

namespace AmberRenderer::Resources
{
	class Material
	{
	public:
		Material(const std::string& p_name = "default");
		~Material();

		void Bind(Texture* p_emptyTexture) const;
		void Unbind() const;

		bool HasShader() const;

		Shader* GetShader() const;
		const Texture* GetTexture() const;

		void SetShader(Shader* p_shader);
		void SetTexture(Texture* p_texture);

	public:
		const std::string Name;

	private:
		Shader* m_shader;
		Texture* m_texture;
	};
}
