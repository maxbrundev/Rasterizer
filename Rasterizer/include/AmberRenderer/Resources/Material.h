#pragma once

#include <string>

#include "AmberRenderer/Rendering/SoftwareRenderer/Programs//AProgram.h"

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

		Rendering::SoftwareRenderer::Programs::AProgram* GetShader() const;
		const Texture* GetTexture() const;

		void SetShader(Rendering::SoftwareRenderer::Programs::AProgram* p_shader);
		void SetTexture(Texture* p_texture);

	public:
		const std::string Name;

	private:
		Rendering::SoftwareRenderer::Programs::AProgram* m_shader;
		Texture* m_texture;
	};
}
