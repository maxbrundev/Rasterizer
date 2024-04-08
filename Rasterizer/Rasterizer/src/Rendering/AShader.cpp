#include "Rendering/AShader.h"

#include <algorithm>

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

void Rendering::AShader::SetSample(const std::string& p_name, Resources::Texture* p_texture)
{
	m_samples[p_name] = p_texture;
}

glm::vec4 Rendering::AShader::Texture(const Resources::Texture& p_texture, const glm::vec2& p_textCoords) const
{
	int uvX = abs(static_cast<int>((p_textCoords.x / m_interpolatedReciprocal) * p_texture.width)) % p_texture.width;
	int uvY = abs(static_cast<int>((p_textCoords.y / m_interpolatedReciprocal) * p_texture.height)) % p_texture.height;

	const int index = (uvY * p_texture.width + uvX) * 4;

	return glm::vec4(glm::vec3(p_texture.data[index] / 255.0f, p_texture.data[index + 1] / 255.0f, p_texture.data[index + 2] / 255.0f), 1.0f);
}
