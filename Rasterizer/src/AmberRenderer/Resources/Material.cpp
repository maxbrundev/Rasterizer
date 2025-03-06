#include "AmberRenderer/Resources/Material.h"

#include "AmberRenderer/Rendering/Rasterizer/GLRasterizer.h"

#include "AmberRenderer/Resources/Loaders/TextureLoader.h"

AmberRenderer::Resources::Material::Material(const std::string& p_name) :
Name(p_name),
m_shader(nullptr),
m_texture(nullptr)
{
}

AmberRenderer::Resources::Material::~Material()
{
	if (m_texture != nullptr)
	{
		Loaders::TextureLoader::Destroy(m_texture);
	}
}

void AmberRenderer::Resources::Material::Bind(Texture* p_emptyTexture) const
{
	if (HasShader())
	{
		m_shader->Bind();

		if (m_texture != nullptr)
		{
			m_shader->SetSample("u_DiffuseMap", m_texture);
		}
		else
		{
			m_shader->SetSample("u_DiffuseMap", p_emptyTexture);
		}
	}
}

bool AmberRenderer::Resources::Material::HasShader() const
{
	return m_shader != nullptr;
}

AmberRenderer::Rendering::Rasterizer::Shaders::AShader* AmberRenderer::Resources::Material::GetShader() const
{
	return m_shader;
}

const AmberRenderer::Resources::Texture* AmberRenderer::Resources::Material::GetTexture() const
{
	return m_texture;
}

void AmberRenderer::Resources::Material::SetShader(Rendering::Rasterizer::Shaders::AShader* p_shader)
{
	m_shader = p_shader;
}

void AmberRenderer::Resources::Material::SetTexture(Texture* p_texture)
{
	m_texture = p_texture;
}
