#include "OnyxEditor/Resources/Material.h"

#include <iostream>

#include "OnyxEditor/Resources/Loaders/TextureLoader.h"

OnyxEditor::Resources::Material::~Material()
{
	m_uniformsData.clear();

	m_shader = nullptr;
}

void OnyxEditor::Resources::Material::SetShader(AShader* p_shader)
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

void OnyxEditor::Resources::Material::Bind(Texture* p_emptyTexture) const
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
							else if (p_emptyTexture)
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

void OnyxEditor::Resources::Material::Unbind() const
{
	if (HasShader())
	{
		m_shader->Unbind();
	}
}

void OnyxEditor::Resources::Material::FillUniform()
{
	m_uniformsData.clear();

	if (m_shader)
	{
		for (const UniformInfo& uniformInfo : m_shader->Uniforms)
		{
			m_uniformsData.emplace(uniformInfo.Name, uniformInfo.DefaultValue);
		}
	}
}

OnyxEditor::Data::StateMask OnyxEditor::Resources::Material::GenerateStateMask() const
{
	Data::StateMask stateMask;
	stateMask.DepthWriting = m_depthWriting;
	stateMask.DepthTest = m_depthTest;
	stateMask.ColorWriting = m_colorWriting;
	stateMask.Blendable = m_blendable;
	stateMask.FrontFaceCulling = m_frontFaceCulling;
	stateMask.BackFaceCulling = m_backFaceCulling;
	return stateMask;
}

void OnyxEditor::Resources::Material::SetBlendable(bool p_blendable)
{
	m_blendable = p_blendable;
}

void OnyxEditor::Resources::Material::SetBackFaceCulling(bool p_backFaceCulling)
{
	m_backFaceCulling = p_backFaceCulling;
}

void OnyxEditor::Resources::Material::SetFrontFaceCulling(bool p_frontFaceCulling)
{
	m_frontFaceCulling = p_frontFaceCulling;
}

void OnyxEditor::Resources::Material::SetDepthTest(bool p_depthTest)
{
	m_depthTest = p_depthTest;
}

void OnyxEditor::Resources::Material::SetDepthWriting(bool p_depthWriting)
{
	m_depthWriting = p_depthWriting;
}

void OnyxEditor::Resources::Material::SetColorWriting(bool p_colorWriting)
{
	m_colorWriting = p_colorWriting;
}

void OnyxEditor::Resources::Material::SetCastShadows(bool p_castShadows)
{
	m_castShadows = p_castShadows;
}

void OnyxEditor::Resources::Material::SetReceiveShadows(bool p_receiveShadows)
{
	m_receiveShadows = p_receiveShadows;
}

bool OnyxEditor::Resources::Material::IsBlendable() const
{
	return m_blendable;
}

bool OnyxEditor::Resources::Material::HasBackFaceCulling() const
{
	return m_backFaceCulling;
}

bool OnyxEditor::Resources::Material::HasFrontFaceCulling() const
{
	return m_frontFaceCulling;
}

bool OnyxEditor::Resources::Material::HasDepthTest() const
{
	return m_depthTest;
}

bool OnyxEditor::Resources::Material::HasDepthWriting() const
{
	return m_depthWriting;
}

bool OnyxEditor::Resources::Material::HasColorWriting() const
{
	return m_colorWriting;
}

bool OnyxEditor::Resources::Material::IsShadowCaster() const
{
	return m_castShadows;
}

bool OnyxEditor::Resources::Material::IsShadowReceiver() const
{
	return m_receiveShadows;
}

OnyxEditor::Resources::AShader* OnyxEditor::Resources::Material::GetShader() const
{
	return m_shader;
}

const std::string& OnyxEditor::Resources::Material::GetName()
{
	return m_name;
}

bool OnyxEditor::Resources::Material::HasShader() const
{
	return m_shader != nullptr;
}

bool OnyxEditor::Resources::Material::HasUniform(const std::string& p_name) const
{
	return m_uniformsData.contains(p_name);
}

void OnyxEditor::Resources::Material::SetName(const std::string& p_name)
{
	m_name = p_name;
}
