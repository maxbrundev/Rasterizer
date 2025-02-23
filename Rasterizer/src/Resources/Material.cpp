#include "Resources/Material.h"

#include "Rendering/Rasterizer/GLRasterizer.h"

#include "Resources/Loaders/TextureLoader.h"

Resources::Material::Material(const std::string& p_name) :
Name(p_name),
m_shader(nullptr),
m_texture(nullptr)
{
}

Resources::Material::~Material()
{
	if (m_texture != nullptr)
	{
		Loaders::TextureLoader::Destroy(m_texture);
	}
}

void Resources::Material::Bind(Texture* p_emptyTexture) const
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

bool Resources::Material::HasShader() const
{
	return m_shader != nullptr;
}

Rendering::Rasterizer::Shaders::AShader* Resources::Material::GetShader() const
{
	return m_shader;
}

const Resources::Texture* Resources::Material::GetTexture() const
{
	return m_texture;
}

void Resources::Material::SetShader(Rendering::Rasterizer::Shaders::AShader* p_shader)
{
	m_shader = p_shader;
}

void Resources::Material::SetTexture(Texture* p_texture)
{
	m_texture = p_texture;
}
