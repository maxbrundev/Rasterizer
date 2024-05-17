#include "Rendering/AShader.h"

glm::vec4 Rendering::AShader::ProcessVertex(const Geometry::Vertex& p_vertex, uint8_t p_vertexID)
{
	m_vertexIndex = p_vertexID;

	return VertexPass(p_vertex);
}

void Rendering::AShader::ProcessInterpolation(const glm::vec3& p_barycentricCoords, float p_w0, float p_w1, float p_w2)
{
	for (const auto&[key, value] : m_varying[0])
	{
		if (auto data = std::get_if<glm::vec3>(&m_varying[0].at(key)); data != nullptr)
		{
			InterpolateData(key, *data, std::get<glm::vec3>(m_varying[1].at(key)), std::get<glm::vec3>(m_varying[2].at(key)), p_barycentricCoords, p_w0, p_w1, p_w2);
		}
		else if (auto data = std::get_if<glm::vec4>(&m_varying[0].at(key)); data != nullptr)
		{
			InterpolateData(key, *data, std::get<glm::vec4>(m_varying[1].at(key)), std::get<glm::vec4>(m_varying[2].at(key)), p_barycentricCoords, p_w0, p_w1, p_w2);
		}
		else if (auto data = std::get_if<float>(&m_varying[0].at(key)); data != nullptr)
		{
			InterpolateData(key, *data, std::get<float>(m_varying[1].at(key)), std::get<float>(m_varying[2].at(key)), p_barycentricCoords, p_w0, p_w1, p_w2);
		}
		else if (auto data = std::get_if<glm::vec2>(&m_varying[0].at(key)); data != nullptr)
		{
			InterpolateData(key, *data, std::get<glm::vec2>(m_varying[1].at(key)), std::get<glm::vec2>(m_varying[2].at(key)), p_barycentricCoords, p_w0, p_w1, p_w2);
		}
		else if (auto data = std::get_if<int>(&m_varying[0].at(key)); data != nullptr)
		{
			InterpolateData(key, *data, std::get<int>(m_varying[1].at(key)), std::get<int>(m_varying[2].at(key)), p_barycentricCoords, p_w0, p_w1, p_w2);
		}
		else if (auto data = std::get_if<glm::mat3>(&m_varying[0].at(key)); data != nullptr)
		{
			InterpolateData(key, *data, std::get<glm::mat3>(m_varying[1].at(key)), std::get<glm::mat3>(m_varying[2].at(key)), p_barycentricCoords, p_w0, p_w1, p_w2);
		}
		else if (auto data = std::get_if<glm::mat4>(&m_varying[0].at(key)); data != nullptr)
		{
			InterpolateData(key, *data, std::get<glm::mat4>(m_varying[1].at(key)), std::get<glm::mat4>(m_varying[2].at(key)), p_barycentricCoords, p_w0, p_w1, p_w2);
		}
		else if (auto data = std::get_if<glm::mat2>(&m_varying[0].at(key)); data != nullptr)
		{
			InterpolateData(key, *data, std::get<glm::mat2>(m_varying[1].at(key)), std::get<glm::mat2>(m_varying[2].at(key)), p_barycentricCoords, p_w0, p_w1, p_w2);
		}
	}

	m_interpolatedReciprocal = (1.0f / p_w0) * p_barycentricCoords.z + (1.0f / p_w1) * p_barycentricCoords.y + (1.0f / p_w2) * p_barycentricCoords.x;
}

Data::Color Rendering::AShader::ProcessFragment()
{
	return FragmentPass();
}

void Rendering::AShader::SetUniform(const std::string_view p_name, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value)
{
	m_uniforms[p_name] = p_value;
}

void Rendering::AShader::SetFlat(const std::string_view p_name, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value)
{
	m_flats[p_name] = p_value;
}

void Rendering::AShader::SetVarying(const std::string_view p_name, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value)
{
	m_varying[m_vertexIndex][p_name] = p_value;
}

void Rendering::AShader::SetVarying(const std::string_view p_name, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value, uint8_t p_index)
{
	m_varying[p_index][p_name] = p_value;
}

void Rendering::AShader::SetSample(const std::string_view p_name, Resources::Texture* p_texture)
{
	m_samples[p_name] = p_texture;
}

uint8_t Rendering::AShader::ComputeCurrentMipmapIndex(uint8_t p_mipmapsCount) const
{
	const auto& u_ViewPos = GetUniform<glm::vec3>("u_ViewPos");
	const auto& v_FragPos = GetFlat<glm::vec3>("v_FragPos");

	const uint8_t maxLevel = p_mipmapsCount - 1;

	const auto viewPosToFragPos = glm::vec3(v_FragPos - u_ViewPos);

	uint8_t distanceRatio = static_cast<uint8_t>(glm::clamp(glm::length(glm::round(viewPosToFragPos)) / MIPMAPS_DISTANCE_STEP, 0.0f, static_cast<float>(maxLevel)));

	return distanceRatio;
}

glm::vec4 Rendering::AShader::Texture(const Resources::Texture& p_texture, const glm::vec2& p_textCoords) const
{
	uint32_t width  = p_texture.Width;
	uint32_t height = p_texture.Height;

	uint8_t currentLOD = 0;

	if(p_texture.HasMipmaps)
	{
		currentLOD = ComputeCurrentMipmapIndex(static_cast<uint8_t>(p_texture.Mipmaps.size()));

		width  = p_texture.Mipmaps[currentLOD].Width;
		height = p_texture.Mipmaps[currentLOD].Height;
	}

	float uvX = abs(p_textCoords.x / m_interpolatedReciprocal);
	float uvY = abs(p_textCoords.y / m_interpolatedReciprocal);

	if (p_texture.Wrapping == Resources::Settings::ETextureWrapMode::CLAMP)
	{
		uvX = glm::clamp(uvX, 0.0f, 1.0f);
		uvY = glm::clamp(uvY, 0.0f, 1.0f);
	}
	else if (p_texture.Wrapping == Resources::Settings::ETextureWrapMode::REPEAT)
	{
		uvX = glm::mod(uvX, 1.0f);
		uvY = glm::mod(uvY, 1.0f);
	}

	uvX = uvX * (static_cast<float>(width) - 0.5f);
	uvY = uvY * (static_cast<float>(height) - 0.5f);

	if (p_texture.Filter == Resources::Settings::ETextureFilteringMode::NEAREST) 
	{
		uvX = std::round(uvX);
		uvY = std::round(uvY);
	}
	else if (p_texture.Filter == Resources::Settings::ETextureFilteringMode::LINEAR) 
	{
		uvX = std::floor(uvX);
		uvY = std::floor(uvY);
	}

	int x = static_cast<int>(uvX);
	int y = static_cast<int>(uvY);

	x = glm::clamp(x, 0, static_cast<int>(width) - 1);
	y = glm::clamp(y, 0, static_cast<int>(height) - 1);

	const uint32_t index = (y * width + x) * 4;

	if(p_texture.HasMipmaps)
	{
		return glm::vec4(glm::vec3(p_texture.Mipmaps[currentLOD].Data[index] / 255.0f, p_texture.Mipmaps[currentLOD].Data[index + 1] / 255.0f, p_texture.Mipmaps[currentLOD].Data[index + 2] / 255.0f), 1.0f);
	}

	return glm::vec4(glm::vec3(p_texture.Data[index] / 255.0f, p_texture.Data[index + 1] / 255.0f, p_texture.Data[index + 2] / 255.0f), 1.0f);
}
