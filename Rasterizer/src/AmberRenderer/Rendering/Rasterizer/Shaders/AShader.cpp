#include "AmberRenderer/Rendering/Rasterizer/Shaders/AShader.h"

#include <glm/gtc/type_ptr.inl>

#include "AmberRenderer/Rendering/Rasterizer/GLRasterizer.h"

static inline void CopyFloats(float* dest, const float* source, int count)
{
	std::memcpy(dest, source, sizeof(float) * count);
}

void AmberRenderer::Rendering::Rasterizer::Shaders::AShader::Bind()
{
	GLRasterizer::UseProgram(this);
}

glm::vec4 AmberRenderer::Rendering::Rasterizer::Shaders::AShader::ProcessVertex(const Geometry::Vertex& p_vertex, uint8_t p_vertexID)
{
	m_vertexIndex = p_vertexID;
	return VertexPass(p_vertex);
}

void AmberRenderer::Rendering::Rasterizer::Shaders::AShader::ProcessInterpolation(const glm::vec3& p_barycentricCoords, float p_w0, float p_w1, float p_w2)
{
	m_interpolatedReciprocal = (1.0f / p_w0) * p_barycentricCoords.x + (1.0f / p_w1) * p_barycentricCoords.y + (1.0f / p_w2) * p_barycentricCoords.z;

	for (auto& [key, varData] : m_varyings)
	{
		int count = GetTypeCount(varData.Type);
		for (int i = 0; i < count; ++i)
		{
			float d0 = varData.Data[0][i];
			float d1 = varData.Data[1][i];
			float d2 = varData.Data[2][i];

			float val = (d0 / p_w0) * p_barycentricCoords.x +
						(d1 / p_w1) * p_barycentricCoords.y +
						(d2 / p_w2) * p_barycentricCoords.z;

			varData.Interpolated[i] = val; // / m_interpolatedReciprocal;
		}
	}
}

AmberRenderer::Data::Color AmberRenderer::Rendering::Rasterizer::Shaders::AShader::ProcessFragment()
{
	return FragmentPass();
}

int AmberRenderer::Rendering::Rasterizer::Shaders::AShader::GetTypeCount(EShaderDataType p_type) const 
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

glm::vec4 AmberRenderer::Rendering::Rasterizer::Shaders::AShader::Texture(const std::string_view p_samplerName, const glm::vec2& p_texCoords) const
{
	int textureUnit = GetUniformAs<int>(p_samplerName);

	TextureObject* textureObject = GLRasterizer::GetTextureObject(static_cast<uint32_t>(textureUnit));

	if (textureObject == nullptr)
		return glm::vec4(1.0f);

	uint32_t width = textureObject->Width;
	uint32_t height = textureObject->Height;

	uint8_t currentLOD = 0;

	float uvX;
	float uvY;

	if (textureObject->InternalFormat == GLR_DEPTH_COMPONENT)
	{
		uvX = abs(p_texCoords.x);
		uvY = abs(p_texCoords.y);
	}
	else
	{
		uvX = abs(p_texCoords.x / m_interpolatedReciprocal);
		uvY = abs(p_texCoords.y / m_interpolatedReciprocal);
	}

	if (textureObject->WrapS == Resources::Settings::ETextureWrapMode::CLAMP)
	{
		uvX = glm::clamp(uvX, 0.0f, 1.0f);
		uvY = glm::clamp(uvY, 0.0f, 1.0f);
	}
	else if (textureObject->WrapS == Resources::Settings::ETextureWrapMode::REPEAT)
	{
		uvX = glm::mod(uvX, 1.0f);
		uvY = glm::mod(uvY, 1.0f);
	}

	uvX = uvX * (static_cast<float>(width) - 0.5f);
	uvY = uvY * (static_cast<float>(height) - 0.5f);

	if (textureObject->MinFilter == Resources::Settings::ETextureFilteringMode::NEAREST)
	{
		uvX = std::round(uvX);
		uvY = std::round(uvY);
	}
	else if (textureObject->MinFilter == Resources::Settings::ETextureFilteringMode::LINEAR)
	{
		uvX = std::floor(uvX);
		uvY = std::floor(uvY);
	}

	int x = static_cast<int>(uvX);
	int y = static_cast<int>(uvY);

	x = glm::clamp(x, 0, static_cast<int>(width) - 1);
	y = glm::clamp(y, 0, static_cast<int>(height) - 1);

	const uint32_t index = (y * width + x) * 4;

	if (textureObject->InternalFormat == GLR_DEPTH_COMPONENT)
	{
		//return glm::vec4(glm::vec3(texObj->Data32[index] * 255.0f, texObj->Data32[index + 1] * 255.0f, texObj->Data32[index + 2] * 255.0f), 1.0f);
	}

	return glm::vec4(glm::vec3(textureObject->Data8[index] / 255.0f, textureObject->Data8[index + 1] / 255.0f, textureObject->Data8[index + 2] / 255.0f), 1.0f);
}

glm::vec3 AmberRenderer::Rendering::Rasterizer::Shaders::AShader::Lambert(const glm::vec3& p_fragPos, const glm::vec3& p_normal, const glm::vec3& p_lightPos, const glm::vec3& p_lightDiffuse, const glm::vec3& p_lightAmbient) const
{
	const float diffuse = glm::max(glm::dot(p_normal, glm::normalize(p_lightPos - p_fragPos)), 0.0f);
	return glm::clamp(p_lightDiffuse * diffuse + p_lightAmbient, 0.0f, 1.0f);
}

uint8_t AmberRenderer::Rendering::Rasterizer::Shaders::AShader::ComputeCurrentMipmapIndex(uint8_t p_mipmapsCount) const
{
	const auto& u_ViewPos = GetUniformAs<glm::vec3>("u_ViewPos");
	const auto& v_FragPos = GetVaryingAs<glm::vec3>("v_FragPos");

	const uint8_t maxLevel = p_mipmapsCount - 1;

	const auto viewPosToFragPos = glm::vec3(v_FragPos - u_ViewPos);

	uint8_t distanceRatio = static_cast<uint8_t>(glm::clamp(glm::length(glm::round(viewPosToFragPos)) / MIPMAPS_DISTANCE_STEP, 0.0f, static_cast<float>(maxLevel)));

	return distanceRatio;
}
