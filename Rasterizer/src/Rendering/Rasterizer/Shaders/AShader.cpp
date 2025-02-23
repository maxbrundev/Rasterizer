#include "Rendering/Rasterizer/Shaders/AShader.h"

#include <glm/gtc/type_ptr.inl>

#include "Rendering/Rasterizer/GLRasterizer.h"

static inline void CopyFloats(float* dest, const float* source, int count)
{
	std::memcpy(dest, source, sizeof(float) * count);
}

void Rendering::Rasterizer::Shaders::AShader::Bind()
{
	GLRasterizer::UseProgram(this);
}

glm::vec4 Rendering::Rasterizer::Shaders::AShader::ProcessVertex(const Geometry::Vertex& p_vertex, uint8_t p_vertexID)
{
	m_vertexIndex = p_vertexID;
	return VertexPass(p_vertex);
}

void Rendering::Rasterizer::Shaders::AShader::ProcessInterpolation(const glm::vec3& p_barycentricCoords, float p_w0, float p_w1, float p_w2)
{
	for (auto& [key, varData] : m_varyings)
	{
		int count = GetTypeCount(varData.Type);
		for (int i = 0; i < count; ++i)
		{
			float d0 = varData.Data[0][i];
			float d1 = varData.Data[1][i];
			float d2 = varData.Data[2][i];

			float val = (d0 / p_w0) * p_barycentricCoords.z +
						(d1 / p_w1) * p_barycentricCoords.y +
						(d2 / p_w2) * p_barycentricCoords.x;

			varData.Interpolated[i] = val;
		}
	}

	m_interpolatedReciprocal = (1.0f / p_w0) * p_barycentricCoords.z + (1.0f / p_w1) * p_barycentricCoords.y + (1.0f / p_w2) * p_barycentricCoords.x;
}

Data::Color Rendering::Rasterizer::Shaders::AShader::ProcessFragment()
{
	return FragmentPass();
}

void Rendering::Rasterizer::Shaders::AShader::SetUniformInt(const std::string_view p_name, int p_value)
{
	ShaderData& shaderData = m_uniforms[p_name];
	shaderData.Type = EShaderDataType::INT;
	shaderData.Data[0] = static_cast<float>(p_value);
}

void Rendering::Rasterizer::Shaders::AShader::SetUniformFloat(const std::string_view p_name, float p_value)
{
	ShaderData& shaderData = m_uniforms[p_name];
	shaderData.Type = EShaderDataType::FLOAT;
	shaderData.Data[0] = p_value;
}

void Rendering::Rasterizer::Shaders::AShader::SetUniformVec2(const std::string_view p_name, const glm::vec2& p_value)
{
	ShaderData& shaderData = m_uniforms[p_name];
	shaderData.Type = EShaderDataType::VEC2;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 2);
}

void Rendering::Rasterizer::Shaders::AShader::SetUniformVec3(const std::string_view p_name, const glm::vec3& p_value)
{
	ShaderData& shaderData = m_uniforms[p_name];
	shaderData.Type = EShaderDataType::VEC3;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 3);
}

void Rendering::Rasterizer::Shaders::AShader::SetUniformVec4(const std::string_view p_name, const glm::vec4& p_value)
{
	ShaderData& shaderData = m_uniforms[p_name];
	shaderData.Type = EShaderDataType::VEC4;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 4);
}

void Rendering::Rasterizer::Shaders::AShader::SetUniformMat2(const std::string_view p_name, const glm::mat2& p_value)
{
	ShaderData& shaderData = m_uniforms[p_name];
	shaderData.Type = EShaderDataType::MAT2;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 4);
}

void Rendering::Rasterizer::Shaders::AShader::SetUniformMat3(const std::string_view p_name, const glm::mat3& p_value)
{
	ShaderData& shaderData = m_uniforms[p_name];
	shaderData.Type = EShaderDataType::MAT3;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 9);
}

void Rendering::Rasterizer::Shaders::AShader::SetUniformMat4(const std::string_view p_name, const glm::mat4& p_value)
{
	ShaderData& shaderData = m_uniforms[p_name];
	shaderData.Type = EShaderDataType::MAT4;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 16);
}

int Rendering::Rasterizer::Shaders::AShader::GetUniformAsInt(const std::string_view p_name) const
{
	auto it = m_uniforms.find(p_name);

	if (it == m_uniforms.end())
		return 0;

	return static_cast<int>(it->second.Data[0]);
}

float Rendering::Rasterizer::Shaders::AShader::GetUniformAsFloat(const std::string_view p_name) const
{
	auto it = m_uniforms.find(p_name);

	if (it == m_uniforms.end()) 
		return 0.0f;

	return it->second.Data[0];
}

glm::vec2 Rendering::Rasterizer::Shaders::AShader::GetUniformAsVec2(const std::string_view p_name) const
{
	auto it = m_uniforms.find(p_name);

	if (it == m_uniforms.end()) 
		return glm::vec2(0.0f);

	return glm::vec2(it->second.Data[0], it->second.Data[1]);
}

glm::vec3 Rendering::Rasterizer::Shaders::AShader::GetUniformAsVec3(const std::string_view p_name) const
{
	auto it = m_uniforms.find(p_name);

	if (it == m_uniforms.end()) 
		return glm::vec3(0.0f);

	return glm::vec3(it->second.Data[0], it->second.Data[1], it->second.Data[2]);
}

glm::vec4 Rendering::Rasterizer::Shaders::AShader::GetUniformAsVec4(const std::string_view p_name) const
{
	auto it = m_uniforms.find(p_name);

	if (it == m_uniforms.end()) 
		return glm::vec4(0.0f);

	return glm::vec4(
		it->second.Data[0],
		it->second.Data[1],
		it->second.Data[2],
		it->second.Data[3]
	);
}

glm::mat2 Rendering::Rasterizer::Shaders::AShader::GetUniformAsMat2(const std::string_view p_name) const
{
	auto it = m_uniforms.find(p_name);

	if (it == m_uniforms.end()) 
		return glm::mat2(1.0f);

	const float* uniformData = it->second.Data;

	return glm::mat2(uniformData[0], uniformData[1],
	                 uniformData[2], uniformData[3]);
}

glm::mat3 Rendering::Rasterizer::Shaders::AShader::GetUniformAsMat3(const std::string_view p_name) const
{
	auto it = m_uniforms.find(p_name);

	if (it == m_uniforms.end()) 
		return glm::mat3(1.0f);

	const float* uniformData = it->second.Data;

	return glm::mat3(
		uniformData[0], uniformData[1], uniformData[2],
		uniformData[3], uniformData[4], uniformData[5],
		uniformData[6], uniformData[7], uniformData[8]
	);
}

glm::mat4 Rendering::Rasterizer::Shaders::AShader::GetUniformAsMat4(const std::string_view p_name) const
{
	auto it = m_uniforms.find(p_name);

	if (it == m_uniforms.end()) 
		return glm::mat4(1.0f);

	const float* uniformData = it->second.Data;

	return glm::mat4(
		uniformData[0], uniformData[1], uniformData[2], uniformData[3],
		uniformData[4], uniformData[5], uniformData[6], uniformData[7],
		uniformData[8], uniformData[9], uniformData[10], uniformData[11],
		uniformData[12], uniformData[13], uniformData[14], uniformData[15]
	);
}

void Rendering::Rasterizer::Shaders::AShader::SetFlatInt(const std::string_view p_name, int p_value)
{
	ShaderData& shaderData = m_flats[p_name];
	shaderData.Type = EShaderDataType::INT;
	shaderData.Data[0] = static_cast<float>(p_value);
}

void Rendering::Rasterizer::Shaders::AShader::SetFlatFloat(const std::string_view p_name, float p_value)
{
	ShaderData& shaderData = m_flats[p_name];
	shaderData.Type = EShaderDataType::FLOAT;
	shaderData.Data[0] = p_value;
}

void Rendering::Rasterizer::Shaders::AShader::SetFlatVec2(const std::string_view p_name, const glm::vec2& p_value)
{
	ShaderData& shaderData = m_flats[p_name];
	shaderData.Type = EShaderDataType::VEC2;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 2);
}

void Rendering::Rasterizer::Shaders::AShader::SetFlatVec3(const std::string_view p_name, const glm::vec3& p_value)
{
	ShaderData& shaderData = m_flats[p_name];
	shaderData.Type = EShaderDataType::VEC3;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 3);
}

void Rendering::Rasterizer::Shaders::AShader::SetFlatVec4(const std::string_view p_name, const glm::vec4& p_value)
{
	ShaderData& shaderData = m_flats[p_name];
	shaderData.Type = EShaderDataType::VEC4;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 4);
}

void Rendering::Rasterizer::Shaders::AShader::SetFlatMat2(const std::string_view p_name, const glm::mat2& p_value)
{
	ShaderData& shaderData = m_flats[p_name];
	shaderData.Type = EShaderDataType::MAT2;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 4);
}

void Rendering::Rasterizer::Shaders::AShader::SetFlatMat3(const std::string_view p_name, const glm::mat3& p_value)
{
	ShaderData& shaderData = m_flats[p_name];
	shaderData.Type = EShaderDataType::MAT3;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 9);
}

void Rendering::Rasterizer::Shaders::AShader::SetFlatMat4(const std::string_view p_name, const glm::mat4& p_value)
{
	ShaderData& shaderData = m_flats[p_name];
	shaderData.Type = EShaderDataType::MAT4;
	CopyFloats(shaderData.Data, glm::value_ptr(p_value), 16);
}

int Rendering::Rasterizer::Shaders::AShader::GetFlatAsInt(const std::string_view name) const
{
	auto it = m_flats.find(name);

	if (it == m_flats.end())
		return 0;

	return static_cast<int>(it->second.Data[0]);
}

float Rendering::Rasterizer::Shaders::AShader::GetFlatAsFloat(const std::string_view p_name) const
{
	auto it = m_flats.find(p_name);

	if (it == m_flats.end()) 
		return 0.0f;

	return it->second.Data[0];
}

glm::vec2 Rendering::Rasterizer::Shaders::AShader::GetFlatAsVec2(const std::string_view p_name) const
{
	auto it = m_flats.find(p_name);

	if (it == m_flats.end()) 
		return glm::vec2(0.0f);

	return glm::vec2(
		it->second.Data[0],
		it->second.Data[1]
	);
}

glm::vec3 Rendering::Rasterizer::Shaders::AShader::GetFlatAsVec3(const std::string_view p_name) const
{
	auto it = m_flats.find(p_name);

	if (it == m_flats.end()) 
		return glm::vec3(0.0f);

	return glm::vec3(
		it->second.Data[0],
		it->second.Data[1],
		it->second.Data[2]
	);
}

glm::vec4 Rendering::Rasterizer::Shaders::AShader::GetFlatAsVec4(const std::string_view p_name) const
{
	auto it = m_flats.find(p_name);

	if (it == m_flats.end()) 
		return glm::vec4(0.0f);

	return glm::vec4(
		it->second.Data[0],
		it->second.Data[1],
		it->second.Data[2],
		it->second.Data[3]
	);
}

glm::mat2 Rendering::Rasterizer::Shaders::AShader::GetFlatAsMat2(const std::string_view p_name) const
{
	auto it = m_flats.find(p_name);

	if (it == m_flats.end()) 
		return glm::mat2(1.0f);

	const float* flatData = it->second.Data;
	return glm::mat2(flatData[0], flatData[1],
	                 flatData[2], flatData[3]);
}

glm::mat3 Rendering::Rasterizer::Shaders::AShader::GetFlatAsMat3(const std::string_view p_name) const
{
	auto it = m_flats.find(p_name);

	if (it == m_flats.end()) 
		return glm::mat3(1.0f);

	const float* flatData = it->second.Data;
	return glm::mat3(
		flatData[0], flatData[1], flatData[2],
		flatData[3], flatData[4], flatData[5],
		flatData[6], flatData[7], flatData[8]
	);
}

glm::mat4 Rendering::Rasterizer::Shaders::AShader::GetFlatAsMat4(const std::string_view p_name) const
{
	auto it = m_flats.find(p_name);

	if (it == m_flats.end()) 
		return glm::mat4(1.0f);

	const float* flatData = it->second.Data;
	return glm::mat4(
		flatData[0], flatData[1], flatData[2], flatData[3],
		flatData[4], flatData[5], flatData[6], flatData[7],
		flatData[8], flatData[9], flatData[10], flatData[11],
		flatData[12], flatData[13], flatData[14], flatData[15]
	);
}

void Rendering::Rasterizer::Shaders::AShader::SetVaryingInt(const std::string_view p_name, int p_value, uint8_t p_vertexIndex)
{
	if (p_vertexIndex == 255) p_vertexIndex = m_vertexIndex;
	ShaderVarying& var = m_varyings[p_name];
	var.Type = EShaderDataType::INT;
	var.Data[p_vertexIndex][0] = static_cast<float>(p_value);
}

void Rendering::Rasterizer::Shaders::AShader::SetVaryingFloat(const std::string_view p_name, float p_value, uint8_t p_vertexIndex)
{
	if (p_vertexIndex == 255) p_vertexIndex = m_vertexIndex;
	ShaderVarying& shaderVarying = m_varyings[p_name];
	shaderVarying.Type = EShaderDataType::FLOAT;
	shaderVarying.Data[p_vertexIndex][0] = p_value;
}

void Rendering::Rasterizer::Shaders::AShader::SetVaryingVec2(const std::string_view p_name, const glm::vec2& p_value, uint8_t p_vertexIndex)
{
	if (p_vertexIndex == 255) p_vertexIndex = m_vertexIndex;
	ShaderVarying& shaderVarying = m_varyings[p_name];
	shaderVarying.Type = EShaderDataType::VEC2;
	CopyFloats(shaderVarying.Data[p_vertexIndex], glm::value_ptr(p_value), 2);
}

void Rendering::Rasterizer::Shaders::AShader::SetVaryingVec3(const std::string_view p_name, const glm::vec3& p_value, uint8_t p_vertexIndex)
{
	if (p_vertexIndex == 255) p_vertexIndex = m_vertexIndex;
	ShaderVarying& shaderVarying = m_varyings[p_name];
	shaderVarying.Type = EShaderDataType::VEC3;
	CopyFloats(shaderVarying.Data[p_vertexIndex], glm::value_ptr(p_value), 3);
}

void Rendering::Rasterizer::Shaders::AShader::SetVaryingVec4(const std::string_view p_name, const glm::vec4& p_value, uint8_t p_vertexIndex)
{
	if (p_vertexIndex == 255) p_vertexIndex = m_vertexIndex;
	ShaderVarying& shaderVarying = m_varyings[p_name];
	shaderVarying.Type = EShaderDataType::VEC4;
	CopyFloats(shaderVarying.Data[p_vertexIndex], glm::value_ptr(p_value), 4);
}

void Rendering::Rasterizer::Shaders::AShader::SetVaryingMat2(const std::string_view p_name, const glm::mat2& p_value, uint8_t p_vertexIndex)
{
	if (p_vertexIndex == 255) p_vertexIndex = m_vertexIndex;
	ShaderVarying& shaderVarying = m_varyings[p_name];
	shaderVarying.Type = EShaderDataType::MAT2;
	CopyFloats(shaderVarying.Data[p_vertexIndex], glm::value_ptr(p_value), 4);
}

void Rendering::Rasterizer::Shaders::AShader::SetVaryingMat3(const std::string_view p_name, const glm::mat3& p_value, uint8_t p_vertexIndex)
{
	if (p_vertexIndex == 255) p_vertexIndex = m_vertexIndex;
	ShaderVarying& shaderVarying = m_varyings[p_name];
	shaderVarying.Type = EShaderDataType::MAT3;
	CopyFloats(shaderVarying.Data[p_vertexIndex], glm::value_ptr(p_value), 9);
}

void Rendering::Rasterizer::Shaders::AShader::SetVaryingMat4(const std::string_view p_name, const glm::mat4& p_value, uint8_t p_vertexIndex)
{
	if (p_vertexIndex == 255) p_vertexIndex = m_vertexIndex;
	ShaderVarying& shaderVarying = m_varyings[p_name];
	shaderVarying.Type = EShaderDataType::MAT4;
	CopyFloats(shaderVarying.Data[p_vertexIndex], glm::value_ptr(p_value), 16);
}

int Rendering::Rasterizer::Shaders::AShader::GetVaryingAsInt(const std::string_view p_name) const
{
	auto it = m_varyings.find(p_name);

	if (it == m_varyings.end()) 
		return 0;

	return static_cast<int>(it->second.Interpolated[0]);
}

float Rendering::Rasterizer::Shaders::AShader::GetVaryingAsFloat(const std::string_view p_name) const
{
	auto it = m_varyings.find(p_name);

	if (it == m_varyings.end()) 
		return 0.0f;

	return it->second.Interpolated[0];
}

glm::vec2 Rendering::Rasterizer::Shaders::AShader::GetVaryingAsVec2(const std::string_view p_name) const
{
	auto it = m_varyings.find(p_name);

	if (it == m_varyings.end()) 
		return glm::vec2(0.0f);

	return glm::vec2(
		it->second.Interpolated[0],
		it->second.Interpolated[1]
	);
}

glm::vec3 Rendering::Rasterizer::Shaders::AShader::GetVaryingAsVec3(const std::string_view p_name) const
{
	auto it = m_varyings.find(p_name);
	if (it == m_varyings.end()) 
		return glm::vec3(0.0f);

	return glm::vec3(
		it->second.Interpolated[0],
		it->second.Interpolated[1],
		it->second.Interpolated[2]
	);
}

glm::vec4 Rendering::Rasterizer::Shaders::AShader::GetVaryingAsVec4(const std::string_view p_name) const
{
	auto it = m_varyings.find(p_name);

	if (it == m_varyings.end()) 
		return glm::vec4(0.0f);

	return glm::vec4(
		it->second.Interpolated[0],
		it->second.Interpolated[1],
		it->second.Interpolated[2],
		it->second.Interpolated[3]
	);
}

glm::mat2 Rendering::Rasterizer::Shaders::AShader::GetVaryingAsMat2(const std::string_view p_name) const
{
	auto it = m_varyings.find(p_name);

	if (it == m_varyings.end()) 
		return glm::mat2(1.0f);

	const float* varyingInterpolated = it->second.Interpolated;
	return glm::mat2(
		varyingInterpolated[0], varyingInterpolated[1],
		varyingInterpolated[2], varyingInterpolated[3]
	);
}

glm::mat3 Rendering::Rasterizer::Shaders::AShader::GetVaryingAsMat3(const std::string_view p_name) const
{
	auto it = m_varyings.find(p_name);

	if (it == m_varyings.end()) 
		return glm::mat3(1.0f);

	const float* varyingInterpolated = it->second.Interpolated;
	return glm::mat3(
		varyingInterpolated[0], varyingInterpolated[1], varyingInterpolated[2],
		varyingInterpolated[3], varyingInterpolated[4], varyingInterpolated[5],
		varyingInterpolated[6], varyingInterpolated[7], varyingInterpolated[8]
	);
}

glm::mat4 Rendering::Rasterizer::Shaders::AShader::GetVaryingAsMat4(const std::string_view p_name) const
{
	auto it = m_varyings.find(p_name);

	if (it == m_varyings.end()) 
		return glm::mat4(1.0f);

	const float* varyingInterpolated = it->second.Interpolated;
	return glm::mat4(
		varyingInterpolated[0], varyingInterpolated[1], varyingInterpolated[2], varyingInterpolated[3],
		varyingInterpolated[4], varyingInterpolated[5], varyingInterpolated[6], varyingInterpolated[7],
		varyingInterpolated[8], varyingInterpolated[9], varyingInterpolated[10], varyingInterpolated[11],
		varyingInterpolated[12], varyingInterpolated[13], varyingInterpolated[14], varyingInterpolated[15]
	);
}

void Rendering::Rasterizer::Shaders::AShader::SetSample(const std::string_view p_name, Resources::Texture* p_texture)
{
	m_samples[p_name] = p_texture;
}

Resources::Texture* Rendering::Rasterizer::Shaders::AShader::GetSample(const std::string_view p_name) const
{
	auto it = m_samples.find(p_name);

	if (it == m_samples.end()) 
		return nullptr;

	return it->second;
}

int Rendering::Rasterizer::Shaders::AShader::GetTypeCount(EShaderDataType p_type)
{
	switch (p_type)
	{
	case EShaderDataType::INT:
	case EShaderDataType::FLOAT: return 1;
	case EShaderDataType::VEC2: return 2;
	case EShaderDataType::VEC3: return 3;
	case EShaderDataType::VEC4: return 4;
	case EShaderDataType::MAT2: return 4;
	case EShaderDataType::MAT3: return 9;
	case EShaderDataType::MAT4: return 16;
	default: return 1;
	}
}

uint8_t Rendering::Rasterizer::Shaders::AShader::ComputeCurrentMipmapIndex(uint8_t p_mipmapsCount) const
{
	const auto& u_ViewPos = GetUniformAsVec3("u_ViewPos");
	const auto& v_FragPos = GetFlatAsVec3("v_FragPos");

	const uint8_t maxLevel = p_mipmapsCount - 1;

	const auto viewPosToFragPos = glm::vec3(v_FragPos - u_ViewPos);

	uint8_t distanceRatio = static_cast<uint8_t>(glm::clamp(glm::length(glm::round(viewPosToFragPos)) / MIPMAPS_DISTANCE_STEP, 0.0f, static_cast<float>(maxLevel)));

	return distanceRatio;
}

glm::vec4 Rendering::Rasterizer::Shaders::AShader::Texture(const Resources::Texture& p_texture, const glm::vec2& p_texCoords) const
{
	uint32_t width = p_texture.Width;
	uint32_t height = p_texture.Height;

	uint8_t currentLOD = 0;

	if (p_texture.HasMipmaps)
	{
		currentLOD = ComputeCurrentMipmapIndex(static_cast<uint8_t>(p_texture.Mipmaps.size()));

		width = p_texture.Mipmaps[currentLOD].Width;
		height = p_texture.Mipmaps[currentLOD].Height;
	}

	float uvX = abs(p_texCoords.x / m_interpolatedReciprocal);
	float uvY = abs(p_texCoords.y / m_interpolatedReciprocal);

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

	if (p_texture.HasMipmaps)
	{
		return glm::vec4(glm::vec3(p_texture.Mipmaps[currentLOD].Data[index] / 255.0f,
		                           p_texture.Mipmaps[currentLOD].Data[index + 1] / 255.0f,
		                           p_texture.Mipmaps[currentLOD].Data[index + 2] / 255.0f), 1.0f);
	}

	return glm::vec4(glm::vec3(p_texture.Data[index] / 255.0f, p_texture.Data[index + 1] / 255.0f, p_texture.Data[index + 2] / 255.0f), 1.0f);
}
