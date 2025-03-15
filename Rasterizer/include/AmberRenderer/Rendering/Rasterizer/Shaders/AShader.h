#pragma once

#include <string>
#include <unordered_map>

#include "AmberRenderer/Geometry/Vertex.h"

#include "AmberRenderer/Rendering/Rasterizer/Shaders/EShaderUniformType.h"
#include "AmberRenderer/Rendering/Rasterizer/Shaders/ShaderTypeTraits.h"

#include "AmberRenderer/Resources/Texture.h"

namespace AmberRenderer::Rendering::Rasterizer::Shaders
{
	struct ShaderData
	{
		EShaderDataType Type;
		float Data[16];
	};

	struct ShaderVarying
	{
		EShaderDataType Type;

		float Data[3][16];
		float Interpolated[16]{};
	};

	constexpr float MIPMAPS_DISTANCE_STEP = 5.0f;

	//TODO: Rename to Program.
	class AShader
	{
	public:
		AShader() = default;
		virtual ~AShader() = default;

		void Bind();

		virtual glm::vec4 ProcessVertex(const Geometry::Vertex& p_vertex, uint8_t p_vertexID);

		void ProcessInterpolation(const glm::vec3& p_barycentricCoords, float p_w0, float p_w1, float p_w2);

		Data::Color ProcessFragment();

		template<typename T>
		void SetUniform(const std::string_view p_name, const T& p_value)
		{
			ShaderData& shaderData = m_uniforms[p_name];
			shaderData.Type = ShaderTypeTraits<T>::Type;

			ShaderTypeTraits<T>::WriteToBuffer(p_value, shaderData.Data);
		}

		template<typename T>
		T GetUniformAs(const std::string_view p_name) const
		{
			auto it = m_uniforms.find(p_name);

			if (it == m_uniforms.end())
				return T{};

			const ShaderData& shaderData = it->second;

			return ShaderTypeTraits<T>::ReadFromBuffer(shaderData.Data);
		}

		template<typename T>
		void SetFlat(const std::string_view p_name, const T& p_value)
		{
			ShaderData& shaderData = m_flats[p_name];
			shaderData.Type = ShaderTypeTraits<T>::Type;
			ShaderTypeTraits<T>::WriteToBuffer(p_value, shaderData.Data);
		}

		template<typename T>
		T GetFlatAs(const std::string_view p_name) const
		{
			auto it = m_flats.find(p_name);

			if (it == m_flats.end())
				return T{};

			return ShaderTypeTraits<T>::ReadFromBuffer(it->second.Data);
		}

		template<typename T>
		void SetVarying(const std::string_view p_name, const T& p_value, uint8_t p_vertexIndex = 255)
		{
			if (p_vertexIndex == 255) p_vertexIndex = m_vertexIndex;
			ShaderVarying& shaderVarying = m_varyings[p_name];
			shaderVarying.Type = ShaderTypeTraits<T>::Type;

			ShaderTypeTraits<T>::WriteToBuffer(p_value, shaderVarying.Data[p_vertexIndex]);
		}

		template<typename T>
		T GetVaryingAs(const std::string_view p_name) const
		{
			auto it = m_varyings.find(p_name);
			if (it == m_varyings.end())
				return T{};

			return ShaderTypeTraits<T>::ReadFromBuffer(it->second.Interpolated) / m_interpolatedReciprocal;
		}

		template<typename T>
		T GetVaryingAs(const std::string_view p_name, uint8_t p_vertexIndex) const
		{
			auto it = m_varyings.find(p_name);
			if (it == m_varyings.end())
				return T{};

			return ShaderTypeTraits<T>::ReadFromBuffer(it->second.Data[p_vertexIndex]);
		}

		int GetTypeCount(EShaderDataType p_type) const;

		glm::vec4 Texture(const std::string_view p_samplerName, const glm::vec2& p_texCoords) const;

	protected:
		virtual glm::vec4 VertexPass(const Geometry::Vertex& p_vertex) = 0;
		virtual Data::Color FragmentPass() = 0;

		glm::vec3 Lambert(const glm::vec3& p_fragPos, const glm::vec3& p_normal, const glm::vec3& p_lightPos, const glm::vec3& p_lightDiffuse, const glm::vec3& p_lightAmbient) const;

	protected:
		uint8_t m_vertexIndex = 0;

		float m_interpolatedReciprocal = 1.0f;

	private:
		std::unordered_map<std::string_view, ShaderData> m_uniforms;
		std::unordered_map<std::string_view, ShaderData> m_flats;
		std::unordered_map<std::string_view, ShaderVarying> m_varyings;
	};
}
