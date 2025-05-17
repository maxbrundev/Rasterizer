#include "AmberEditor/Resources/Material.h"

#include "AmberEditor/Resources/Loaders/TextureLoader.h"

AmberEditor::Resources::Material::~Material()
{
	m_uniformsData.clear();

	m_shader = nullptr;
}

void AmberEditor::Resources::Material::Bind(Texture* p_emptyTexture) const
{
	if (HasShader())
	{
		m_shader->Bind();

		int textureSlot = 0;

		for (const auto& [name, value] : m_uniformsData)
		{
			if (const auto uniformData = m_shader->GetUniformInfo(name))
			{
				switch (uniformData->Type)
				{
				case Rendering::Settings::EUniformType::INT:   if (value.type() == typeid(int))       m_shader->SetUniform(name, std::any_cast<int>(value));       break;
				case Rendering::Settings::EUniformType::FLOAT: if (value.type() == typeid(float))     m_shader->SetUniform(name, std::any_cast<float>(value));     break;
				case Rendering::Settings::EUniformType::VEC2:  if (value.type() == typeid(glm::vec2)) m_shader->SetUniform(name, std::any_cast<glm::vec2>(value)); break;
				case Rendering::Settings::EUniformType::VEC3:  if (value.type() == typeid(glm::vec3)) m_shader->SetUniform(name, std::any_cast<glm::vec3>(value)); break;
				case Rendering::Settings::EUniformType::VEC4:  if (value.type() == typeid(glm::vec4)) m_shader->SetUniform(name, std::any_cast<glm::vec4>(value)); break;
				case Rendering::Settings::EUniformType::MAT2:  if (value.type() == typeid(glm::mat2)) m_shader->SetUniform(name, std::any_cast<glm::mat2>(value)); break;
				case Rendering::Settings::EUniformType::MAT3:  if (value.type() == typeid(glm::mat3)) m_shader->SetUniform(name, std::any_cast<glm::mat3>(value)); break;
				case Rendering::Settings::EUniformType::MAT4:  if (value.type() == typeid(glm::mat4)) m_shader->SetUniform(name, std::any_cast<glm::mat4>(value)); break;
				case Rendering::Settings::EUniformType::SAMPLER_2D:
					{
						if (value.type() == typeid(Texture*))
						{
							if (auto texture = std::any_cast<Texture*>(value); texture)
							{
								texture->Bind(textureSlot);
								m_shader->SetUniform(uniformData->Name, textureSlot++);
							}
							else if (p_emptyTexture && uniformData->Name == "u_DiffuseMap")
							{
								p_emptyTexture->Bind(textureSlot);
								m_shader->SetUniform(uniformData->Name, textureSlot++);
							}
						}
					}
					break;
				}
			}
		}
	}
}

void AmberEditor::Resources::Material::Unbind() const
{
	if (HasShader())
	{
		for (auto& [name, value] : m_uniformsData)
		{
			if (const auto uniformData = m_shader->GetUniformInfo(name))
			{
				if (uniformData->Type == Rendering::Settings::EUniformType::SAMPLER_2D)
				{
					if (value.type() == typeid(Texture*))
					{
						if (auto texture = std::any_cast<Texture*>(value); texture)
						{
							texture->Unbind();
						}
					}
				}
			}
		}

		m_shader->Unbind();
	}
}

void AmberEditor::Resources::Material::FillUniform()
{
	m_uniformsData.clear();

	if (m_shader)
	{
		const auto& uniforms = m_shader->GetUniforms();
		for (const UniformInfo& uniformInfo : uniforms)
		{
			m_uniformsData.emplace(uniformInfo.Name, uniformInfo.DefaultValue);
		}
	}
}

bool AmberEditor::Resources::Material::HasShader() const
{
	return m_shader != nullptr;
}

AmberEditor::Resources::AShader* AmberEditor::Resources::Material::GetShader() const
{
	return m_shader;
}

const AmberEditor::Resources::Texture* AmberEditor::Resources::Material::GetTexture() const
{
	return m_texture;
}

const std::string& AmberEditor::Resources::Material::GetName()
{
	return m_name;
}

void AmberEditor::Resources::Material::SetShader(AShader* p_shader)
{
	m_shader = p_shader;

	if (m_shader)
	{
		FillUniform();
	}
	else
	{
		m_uniformsData.clear();
	}
}

void AmberEditor::Resources::Material::SetName(const std::string& p_name)
{
	m_name = p_name;
}
