#pragma once

#include <any>
#include <string>
#include <unordered_map>

#include "AmberGL/API/Export.h"

#include "AmberGL/Geometry/Vertex.h"

#include "AmberGL/SoftwareRenderer/Programs/EShaderUniformType.h"
#include "AmberGL/SoftwareRenderer/Programs/ShaderTypeTraits.h"

namespace AmberGL::SoftwareRenderer::Programs
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

	class API_AMBERGL AProgram
	{
	public:
		AProgram() = default;
		virtual ~AProgram() = default;

		void Bind();

		virtual glm::vec4 ProcessVertex(const Geometry::Vertex& p_vertex, uint8_t p_vertexID);

		void ProcessInterpolation(const glm::vec3& p_barycentricCoords, float p_w0, float p_w1, float p_w2);
		void ProcessPointInterpolation(float p_w0);

		glm::vec4 ProcessFragment();

		template<typename T>
		void RegisterUniform(const std::string& p_name, const T& p_defaultValue)
		{
			EShaderDataType type = ShaderTypeTraits<T>::Type;
			m_registeredUniforms[std::string(p_name)] = { type, p_defaultValue };
		}

		std::unordered_map<std::string, std::pair<EShaderDataType, std::any>>& GetRegisteredUniforms()
		{
			return m_registeredUniforms;
		}

		template<typename T>
		void SetUniform(const std::string p_name, const T& p_value)
		{
			ShaderData& shaderData = m_uniforms[p_name];
			shaderData.Type = ShaderTypeTraits<T>::Type;

			if constexpr (std::is_same_v<T, Sampler2D>)
			{
				ShaderTypeTraits<Sampler2D>::WriteToBuffer(p_value.ID, shaderData.Data);
				RegisterUniform(p_name, p_value.ID);
			}
			else
			{
				ShaderTypeTraits<T>::WriteToBuffer(p_value, shaderData.Data);
				RegisterUniform(p_name, p_value);
			}
		}

		template<typename T>
		T GetUniformAs(const std::string p_name) const
		{
			auto it = m_uniforms.find(p_name);

			if (it == m_uniforms.end())
				return T{};

			const ShaderData& shaderData = it->second;

			return ShaderTypeTraits<T>::ReadFromBuffer(shaderData.Data);
		}

		template<typename T>
		void SetFlat(const std::string p_name, const T& p_value)
		{
			ShaderData& shaderData = m_flats[p_name];
			shaderData.Type = ShaderTypeTraits<T>::Type;
			ShaderTypeTraits<T>::WriteToBuffer(p_value, shaderData.Data);
		}

		template<typename T>
		T GetFlatAs(const std::string p_name) const
		{
			auto it = m_flats.find(p_name);

			if (it == m_flats.end())
				return T{};

			return ShaderTypeTraits<T>::ReadFromBuffer(it->second.Data);
		}

		template<typename T>
		void SetVarying(const std::string p_name, const T& p_value, uint8_t p_vertexIndex = 255)
		{
			if (p_vertexIndex == 255) p_vertexIndex = m_vertexIndex;
			ShaderVarying& shaderVarying = m_varyings[p_name];
			shaderVarying.Type = ShaderTypeTraits<T>::Type;

			ShaderTypeTraits<T>::WriteToBuffer(p_value, shaderVarying.Data[p_vertexIndex]);
		}

		template<typename T>
		T GetVaryingAs(const std::string p_name) const
		{
			auto it = m_varyings.find(p_name);
			if (it == m_varyings.end())
				return T{};

			return ShaderTypeTraits<T>::ReadFromBuffer(it->second.Interpolated) / m_interpolatedReciprocal;
		}

		template<typename T>
		T GetVaryingAs(const std::string p_name, uint8_t p_vertexIndex) const
		{
			auto it = m_varyings.find(p_name);
			if (it == m_varyings.end())
				return T{};

			return ShaderTypeTraits<T>::ReadFromBuffer(it->second.Data[p_vertexIndex]);
		}

		void SetDerivatives(glm::vec2 p_dfdx, glm::vec2 p_dfdy);

		glm::vec4 Texture(const std::string p_samplerName, const glm::vec2& p_texCoords) const;

		std::unordered_map<std::string, ShaderVarying>& GetVaryings();
		uint8_t GetTypeCount(EShaderDataType p_type) const;

	protected:
		virtual glm::vec4 VertexPass(const Geometry::Vertex& p_vertex) = 0;
		virtual glm::vec4 FragmentPass() = 0;

		glm::vec3 Lambert(const glm::vec3& p_fragPos, const glm::vec3& p_normal, const glm::vec3& p_lightPos, const glm::vec3& p_lightDiffuse, const glm::vec3& p_lightAmbient) const;

	protected:
		uint8_t m_vertexIndex = 0;
		float m_interpolatedReciprocal = 1.0f;

		glm::vec2 m_dfdx;
		glm::vec2 m_dfdy;

	private:
		std::unordered_map<std::string, ShaderData> m_uniforms;
		std::unordered_map<std::string, ShaderData> m_flats;
		std::unordered_map<std::string, ShaderVarying> m_varyings;

		std::unordered_map<std::string, std::pair<EShaderDataType, std::any>> m_registeredUniforms;

	};
}
