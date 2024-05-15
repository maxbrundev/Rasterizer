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

	m_interpolatedReciprocal = (1 / p_w0) * p_barycentricCoords.z + (1 / p_w1) * p_barycentricCoords.y + (1 / p_w2) * p_barycentricCoords.x;
}

Data::Color Rendering::AShader::ProcessFragment()
{
	return FragmentPass();
}

void Rendering::AShader::SetUniform(const std::string& p_name, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value)
{
	m_uniforms[p_name] = p_value;
}

void Rendering::AShader::SetFlat(const std::string& p_name, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value)
{
	m_flats[p_name] = p_value;
}

void Rendering::AShader::SetVarying(const std::string& p_name, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value)
{
	m_varying[m_vertexIndex][p_name] = p_value;
}

void Rendering::AShader::SetVarying(const std::string& p_name,
	std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value, uint8_t p_index)
{
	m_varying[p_index][p_name] = p_value;
}

void Rendering::AShader::SetSample(const std::string& p_name, Resources::Texture* p_texture)
{
	m_samples[p_name] = p_texture;
}

glm::vec4 Rendering::AShader::Texture(const Resources::Texture& p_texture, const glm::vec2& p_textCoords) const
{
	auto u_ViewPos = GetUniform<glm::vec3>("u_ViewPos");
	auto v_FragPos = GetFlat<glm::vec3>("v_FragPos");

	const float maxDistance = 100.0f;
	const float minDistance = 0.0f;
	const float maxLevel = static_cast<float>(p_texture.Mipmaps.size() - 1);
	
	auto test = glm::vec3(v_FragPos - u_ViewPos);
	float distanceRatio = glm::clamp((glm::length(test) - minDistance) / (maxDistance - minDistance), 0.0f, 1.0f);

	float levelOfDetail = maxLevel * distanceRatio;

	int level = static_cast<int>(levelOfDetail);

	int width = p_texture.HasMipmaps ? p_texture.Mipmaps[level].Width : p_texture.Width;
	int height = p_texture.HasMipmaps ? p_texture.Mipmaps[level].Height : p_texture.Height;

	float uvX = abs(p_textCoords.x / m_interpolatedReciprocal);
	float uvY = abs(p_textCoords.y / m_interpolatedReciprocal);

	if (p_texture.Wrapping == CLAMP) 
	{
		uvX = glm::clamp(uvX, 0.0f, 1.0f);
		uvY = glm::clamp(uvY, 0.0f, 1.0f);
	}
	else if (p_texture.Wrapping == REPEAT) 
	{
		uvX = glm::mod(uvX, 1.0f);
		uvY = glm::mod(uvY, 1.0f);
	}

	float texelX = uvX * width - 0.5f;
	float texelY = uvY * height - 0.5f;

	if (p_texture.Filter == Resources::ETextureFilteringMode::NEAREST) 
	{
		texelX = std::round(texelX);
		texelY = std::round(texelY);
	}
	else if (p_texture.Filter == Resources::ETextureFilteringMode::LINEAR) 
	{
		texelX = std::floor(texelX);
		texelY = std::floor(texelY);
	}

	int x = static_cast<int>(texelX);
	int y = static_cast<int>(texelY);

	x = glm::clamp(x, 0, width - 1);
	y = glm::clamp(y, 0, height - 1);

	const uint32_t index = (y * width + x) * 4;

	if(p_texture.HasMipmaps)
	{
		return glm::vec4(glm::vec3(p_texture.Mipmaps[level].Data[index] / 255.0f, p_texture.Mipmaps[level].Data[index + 1] / 255.0f, p_texture.Mipmaps[level].Data[index + 2] / 255.0f), 1.0f);
	}

	return glm::vec4(glm::vec3(p_texture.Data[index] / 255.0f, p_texture.Data[index + 1] / 255.0f, p_texture.Data[index + 2] / 255.0f), 1.0f);
}
