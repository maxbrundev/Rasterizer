#include "AmberEditor/Resources/Material.h"

#include "AmberEditor/Resources/Loaders/TextureLoader.h"

AmberEditor::Resources::Material::Material(const std::string& p_name) :
Name(p_name),
m_shader(nullptr),
m_texture(nullptr)
{
}

AmberEditor::Resources::Material::~Material()
{
	if (m_texture != nullptr)
	{
		Loaders::TextureLoader::Destroy(m_texture);
	}
}

void AmberEditor::Resources::Material::Bind(Texture* p_emptyTexture) const
{
	if (HasShader())
	{
		m_shader->Bind();

		if (m_texture != nullptr)
		{
			m_texture->Bind();
			m_shader->SetUniform("u_DiffuseMap", 0);
		}
		else
		{
			p_emptyTexture->Bind();
			m_shader->SetUniform("u_DiffuseMap", 0);
		}
	}
}

void AmberEditor::Resources::Material::Unbind() const
{
	if (m_texture != nullptr)
	{
		m_texture->Unbind();
	}

	if (m_shader != nullptr)
	{
		m_shader->Unbind();
	}
}

bool AmberEditor::Resources::Material::HasShader() const
{
	return m_shader != nullptr;
}

AmberEditor::Resources::Shader* AmberEditor::Resources::Material::GetShader() const
{
	return m_shader;
}

const AmberEditor::Resources::Texture* AmberEditor::Resources::Material::GetTexture() const
{
	return m_texture;
}

void AmberEditor::Resources::Material::SetShader(Shader* p_shader)
{
	m_shader = p_shader;
}

void AmberEditor::Resources::Material::SetTexture(Texture* p_texture)
{
	m_texture = p_texture;
}
