#pragma once

#include <string>
#include <unordered_map>

#include "EShaderUniformType.h"
#include "Geometry/Vertex.h"

#include "Resources/Texture.h"

namespace Rendering
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

	class AShader
	{
	public:
		AShader() = default;
		virtual ~AShader() = default;

		virtual glm::vec4 ProcessVertex(const Geometry::Vertex& p_vertex, uint8_t p_vertexID);

		void ProcessInterpolation(const glm::vec3& p_barycentricCoords, float p_w0, float p_w1, float p_w2);

		Data::Color ProcessFragment();

		void SetUniformInt(const std::string_view p_name, int p_value);
		void SetUniformFloat(const std::string_view p_name, float p_value);
		void SetUniformVec2(const std::string_view p_name, const glm::vec2& p_value);
		void SetUniformVec3(const std::string_view p_name, const glm::vec3& p_value);
		void SetUniformVec4(const std::string_view p_name, const glm::vec4& p_value);
		void SetUniformMat2(const std::string_view p_name, const glm::mat2& p_value);
		void SetUniformMat3(const std::string_view p_name, const glm::mat3& p_value);
		void SetUniformMat4(const std::string_view p_name, const glm::mat4& p_value);

		int GetUniformAsInt(const std::string_view p_name) const;
		float GetUniformAsFloat(const std::string_view p_name) const;
		glm::vec2 GetUniformAsVec2(const std::string_view p_name) const;
		glm::vec3 GetUniformAsVec3(const std::string_view p_name) const;
		glm::vec4 GetUniformAsVec4(const std::string_view p_name) const;
		glm::mat2 GetUniformAsMat2(const std::string_view p_name) const;
		glm::mat3 GetUniformAsMat3(const std::string_view p_name) const;
		glm::mat4 GetUniformAsMat4(const std::string_view p_name) const;

		void SetFlatInt(const std::string_view p_name, int p_value);
		void SetFlatFloat(const std::string_view p_name, float p_value);
		void SetFlatVec2(const std::string_view p_name, const glm::vec2& p_value);
		void SetFlatVec3(const std::string_view p_name, const glm::vec3& p_value);
		void SetFlatVec4(const std::string_view p_name, const glm::vec4& p_value);
		void SetFlatMat2(const std::string_view p_name, const glm::mat2& p_value);
		void SetFlatMat3(const std::string_view p_name, const glm::mat3& p_value);
		void SetFlatMat4(const std::string_view p_name, const glm::mat4& p_value);

		int GetFlatAsInt(const std::string_view name) const;
		float GetFlatAsFloat(const std::string_view p_name) const;
		glm::vec2 GetFlatAsVec2(const std::string_view p_name) const;
		glm::vec3 GetFlatAsVec3(const std::string_view p_name) const;
		glm::vec4 GetFlatAsVec4(const std::string_view p_name) const;
		glm::mat2 GetFlatAsMat2(const std::string_view p_name) const;
		glm::mat3 GetFlatAsMat3(const std::string_view p_name) const;
		glm::mat4 GetFlatAsMat4(const std::string_view p_name) const;

		void SetVaryingInt(const std::string_view p_name, int p_value, uint8_t p_vertexIndex = 255);
		void SetVaryingFloat(const std::string_view p_name, float p_value, uint8_t p_vertexIndex = 255);
		void SetVaryingVec2(const std::string_view p_name, const glm::vec2& p_value, uint8_t p_vertexIndex = 255);
		void SetVaryingVec3(const std::string_view p_name, const glm::vec3& p_value, uint8_t p_vertexIndex = 255);
		void SetVaryingVec4(const std::string_view p_name, const glm::vec4& p_value, uint8_t p_vertexIndex = 255);
		void SetVaryingMat2(const std::string_view p_name, const glm::mat2& p_value, uint8_t p_vertexIndex = 255);
		void SetVaryingMat3(const std::string_view p_name, const glm::mat3& p_value, uint8_t p_vertexIndex = 255);
		void SetVaryingMat4(const std::string_view p_name, const glm::mat4& p_value, uint8_t p_vertexIndex = 255);

		int GetVaryingAsInt(const std::string_view p_name) const;
		float GetVaryingAsFloat(const std::string_view p_name) const;
		glm::vec2 GetVaryingAsVec2(const std::string_view p_name) const;
		glm::vec3 GetVaryingAsVec3(const std::string_view p_name) const;
		glm::vec4 GetVaryingAsVec4(const std::string_view p_name) const;
		glm::mat2 GetVaryingAsMat2(const std::string_view p_name) const;
		glm::mat3 GetVaryingAsMat3(const std::string_view p_name) const;
		glm::mat4 GetVaryingAsMat4(const std::string_view p_name) const;

		void SetSample(const std::string_view p_name, Resources::Texture* p_texture);
		Resources::Texture* GetSample(const std::string_view p_name) const;

		int GetTypeCount(EShaderDataType p_type);

	protected:
		virtual glm::vec4 VertexPass(const Geometry::Vertex& p_vertex) = 0;
		virtual Data::Color FragmentPass() = 0;
		glm::vec4 Texture(const Resources::Texture& p_texture, const glm::vec2& p_textCoords) const;

	private:
		uint8_t ComputeCurrentMipmapIndex(uint8_t p_mipmapsAmount) const;

	protected:
		uint8_t m_vertexIndex = 0;

		float m_interpolatedReciprocal = 1.0f;

	private:
		std::unordered_map<std::string_view, ShaderData> m_uniforms;
		std::unordered_map<std::string_view, ShaderData> m_flats;
		std::unordered_map<std::string_view, ShaderVarying> m_varyings;

		std::unordered_map<std::string_view, Resources::Texture*> m_samples;
	};
}
