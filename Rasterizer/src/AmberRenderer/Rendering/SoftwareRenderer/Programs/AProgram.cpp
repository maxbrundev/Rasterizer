#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/AProgram.h"

#include <glm/gtc/type_ptr.inl>
#include "AmberRenderer/Rendering/SoftwareRenderer/RenderObject/TextureObject.h"
#include "AmberRenderer/Rendering/SoftwareRenderer/AmberGL.h"
#include "AmberRenderer/Resources/Settings/ETextureFilteringMode.h"
#include "AmberRenderer/Resources/Settings/ETextureWrapMode.h"

void AmberRenderer::Rendering::SoftwareRenderer::Programs::AProgram::Bind()
{
	AmberGL::UseProgram(this);
}

glm::vec4 AmberRenderer::Rendering::SoftwareRenderer::Programs::AProgram::ProcessVertex(const Geometry::Vertex& p_vertex, uint8_t p_vertexID)
{
	m_vertexIndex = p_vertexID;
	return VertexPass(p_vertex);
}

void AmberRenderer::Rendering::SoftwareRenderer::Programs::AProgram::ProcessInterpolation(const glm::vec3& p_barycentricCoords, float p_w0, float p_w1, float p_w2)
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

			varData.Interpolated[i] = val;
		}
	}
}

AmberRenderer::Data::Color AmberRenderer::Rendering::SoftwareRenderer::Programs::AProgram::ProcessFragment()
{
	return FragmentPass();
}

glm::vec4 AmberRenderer::Rendering::SoftwareRenderer::Programs::AProgram::Texture(const std::string_view p_samplerName, const glm::vec2& p_texCoords) const
{
	int textureUnit = GetUniformAs<int>(p_samplerName);

	RenderObject::TextureObject* textureObject = AmberGL::GetTextureObject(static_cast<uint32_t>(textureUnit));

	if (textureObject == nullptr)
		return glm::vec4(1.0f);

	uint32_t width = textureObject->Width;
	uint32_t height = textureObject->Height;

	bool hasMipmaps = textureObject->Mipmaps != nullptr;
	uint8_t currentLOD = 0;

	if (hasMipmaps)
	{
		//TODO: Compute LOD based on derivative of texture coordinates.
		const auto& u_ViewPos = GetUniformAs<glm::vec3>("u_ViewPos");
		const auto& v_FragPos = GetVaryingAs<glm::vec3>("v_FragPos");

		int maxLevel = 1 + static_cast<int>(std::floor(std::log2(std::max(width, height))));

		const auto viewPosToFragPos = glm::vec3(v_FragPos - u_ViewPos);

		currentLOD = static_cast<uint8_t>(glm::clamp(
			glm::length(glm::round(viewPosToFragPos)) / MIPMAPS_DISTANCE_STEP,
			0.0f,
			static_cast<float>(maxLevel - 1)));
	}

	if (hasMipmaps && currentLOD > 0)
	{
		width = std::max(1u, width >> currentLOD);
		height = std::max(1u, height >> currentLOD);
	}

	float uvX = abs(p_texCoords.x);
	float uvY = abs(p_texCoords.y);

	if (textureObject->WrapS == Resources::Settings::ETextureWrapMode::CLAMP)
	{
		uvX = glm::clamp(uvX, 0.0f, 1.0f);
	}
	else if (textureObject->WrapS == Resources::Settings::ETextureWrapMode::REPEAT)
	{
		uvX = glm::mod(uvX, 1.0f);
	}

	if (textureObject->WrapT == Resources::Settings::ETextureWrapMode::CLAMP)
	{
		uvY = glm::clamp(uvY, 0.0f, 1.0f);
	}
	else if (textureObject->WrapT == Resources::Settings::ETextureWrapMode::REPEAT)
	{
		uvY = glm::mod(uvY, 1.0f);
	}

	uvX = uvX * (static_cast<float>(width) - 0.5f);
	uvY = uvY * (static_cast<float>(height) - 0.5f);

	uint8_t filter = currentLOD == 0 ? textureObject->MagFilter : textureObject->MinFilter;

	uint8_t* data = hasMipmaps && currentLOD > 0 ? textureObject->Mipmaps[currentLOD] : textureObject->Data8;

	if (textureObject->InternalFormat == GLR_DEPTH_COMPONENT)
	{
		//TODO: Handle float data
	}

	//TODO: Nearest Mipmap Nearest, Linear Mipmap Linear, Linear Mipmap Nearest, Nearest Mipmap Linear

	if (filter == Resources::Settings::ETextureFilteringMode::LINEAR)
	{
		int x0 = static_cast<int>(std::floor(uvX));
		int y0 = static_cast<int>(std::floor(uvY));
		int x1 = std::min(x0 + 1, static_cast<int>(width) - 1);
		int y1 = std::min(y0 + 1, static_cast<int>(height) - 1);

		float fracX = uvX - x0;
		float fracY = uvY - y0;

		uint32_t idx00 = (y0 * width + x0) * 4;
		uint32_t idx01 = (y1 * width + x0) * 4;
		uint32_t idx10 = (y0 * width + x1) * 4;
		uint32_t idx11 = (y1 * width + x1) * 4;

		glm::vec4 color;
		for (int i = 0; i < 4; i++) 
		{
			float c00 = data[idx00 + i] / 255.0f;
			float c01 = data[idx01 + i] / 255.0f;
			float c10 = data[idx10 + i] / 255.0f;
			float c11 = data[idx11 + i] / 255.0f;

			color[i] = (1.0f - fracX) * (1.0f - fracY) * c00 +
				fracX * (1.0f - fracY) * c10 +
				(1.0f - fracX) * fracY * c01 +
				fracX * fracY * c11;
		}

		return color;
	}
	
	int x = static_cast<int>(std::round(uvX));
	int y = static_cast<int>(std::round(uvY));

	x = glm::clamp(x, 0, static_cast<int>(width) - 1);
	y = glm::clamp(y, 0, static_cast<int>(height) - 1);

	const uint32_t index = (y * width + x) * 4;

	return glm::vec4(
		data[index] / 255.0f,
		data[index + 1] / 255.0f,
		data[index + 2] / 255.0f,
		data[index + 3] / 255.0f
	);
}

glm::vec3 AmberRenderer::Rendering::SoftwareRenderer::Programs::AProgram::Lambert(const glm::vec3& p_fragPos, const glm::vec3& p_normal, const glm::vec3& p_lightPos, const glm::vec3& p_lightDiffuse, const glm::vec3& p_lightAmbient) const
{
	const float diffuse = glm::max(glm::dot(p_normal, glm::normalize(p_lightPos - p_fragPos)), 0.0f);
	return glm::clamp(p_lightDiffuse * diffuse + p_lightAmbient, 0.0f, 1.0f);
}

std::unordered_map<std::string_view, AmberRenderer::Rendering::SoftwareRenderer::Programs::ShaderVarying>& AmberRenderer::Rendering::SoftwareRenderer::Programs::AProgram::GetVaryings()
{
	return m_varyings;
}

uint8_t AmberRenderer::Rendering::SoftwareRenderer::Programs::AProgram::GetTypeCount(EShaderDataType p_type) const
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
