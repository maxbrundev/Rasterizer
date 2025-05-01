#include "AmberGL/SoftwareRenderer/Programs/AProgram.h"

#include <glm/gtc/type_ptr.inl>

#include "AmberGL/SoftwareRenderer/AmberGL.h"
#include "AmberGL/SoftwareRenderer/TextureSampler.h"
#include "AmberGL/SoftwareRenderer/RenderObject/TextureObject.h"

void AmberGL::SoftwareRenderer::Programs::AProgram::Bind()
{
	AmberGL::UseProgram(this);
}

glm::vec4 AmberGL::SoftwareRenderer::Programs::AProgram::ProcessVertex(const Geometry::Vertex& p_vertex, uint8_t p_vertexID)
{
	m_vertexIndex = p_vertexID;
	return VertexPass(p_vertex);
}

void AmberGL::SoftwareRenderer::Programs::AProgram::ProcessInterpolation(const glm::vec3& p_barycentricCoords, float p_w0, float p_w1, float p_w2)
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

void AmberGL::SoftwareRenderer::Programs::AProgram::ProcessPointInterpolation(float p_w0)
{
	m_interpolatedReciprocal = 1.0f / p_w0;

	for (auto& [key, varData] : m_varyings)
	{
		int count = GetTypeCount(varData.Type);
		for (int i = 0; i < count; i++)
		{
			float d0 = varData.Data[0][i];
			varData.Interpolated[i] = d0 / p_w0;
		}
	}
}

glm::vec4 AmberGL::SoftwareRenderer::Programs::AProgram::ProcessFragment()
{
	return FragmentPass();
}

void AmberGL::SoftwareRenderer::Programs::AProgram::SetDerivatives(glm::vec2 p_dfdx, glm::vec2 p_dfdy)
{
	m_dfdx = p_dfdx;
	m_dfdy = p_dfdy;
}

glm::vec4 AmberGL::SoftwareRenderer::Programs::AProgram::Texture(const std::string_view p_samplerName, const glm::vec2& p_texCoords) const
{
	int textureUnit = GetUniformAs<int>(p_samplerName);

	RenderObject::TextureObject* textureObject = AmberGL::GetTextureObject(static_cast<uint32_t>(textureUnit));

	if (textureObject == nullptr)
		return glm::vec4(1.0f);

	return TextureSampler::Sample(textureObject, p_texCoords, {m_dfdx, m_dfdy });
}

glm::vec3 AmberGL::SoftwareRenderer::Programs::AProgram::Lambert(const glm::vec3& p_fragPos, const glm::vec3& p_normal, const glm::vec3& p_lightPos, const glm::vec3& p_lightDiffuse, const glm::vec3& p_lightAmbient) const
{
	const float diffuse = glm::max(glm::dot(p_normal, glm::normalize(p_lightPos - p_fragPos)), 0.0f);
	return glm::clamp(p_lightDiffuse * diffuse + p_lightAmbient, 0.0f, 1.0f);
}

std::unordered_map<std::string_view, AmberGL::SoftwareRenderer::Programs::ShaderVarying>& AmberGL::SoftwareRenderer::Programs::AProgram::GetVaryings()
{
	return m_varyings;
}

uint8_t AmberGL::SoftwareRenderer::Programs::AProgram::GetTypeCount(EShaderDataType p_type) const
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
