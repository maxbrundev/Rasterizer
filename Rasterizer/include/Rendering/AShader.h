#pragma once

#include <string>
#include <unordered_map>
#include <variant>

#include "Geometry/Vertex.h"

#include "Resources/Texture.h"

namespace Rendering
{
	constexpr float MIPMAPS_DISTANCE_STEP = 20.0f;

	//TODO: Tested multiple options for Shader class data containers implementation: std::any, reinterpret cast, polymorphism, template parameters (Uniform, Varying, Atttributes struct).
	// for now variant is the more straight forward but the performance cost in debug is high, continuing investigation to improve the architecture and performance for shader class.
	class AShader
	{
	public:
		AShader() = default;
		virtual ~AShader() = default;

		virtual glm::vec4 ProcessVertex(const Geometry::Vertex& p_vertex, uint8_t p_vertexID);
		void ProcessInterpolation(const glm::vec3& p_barycentricCoords, float p_w0, float p_w1, float p_w2);
		Data::Color ProcessFragment();

		void SetUniform(const std::string_view p_name, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value);
		void SetFlat(const std::string_view p_name, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value);
		void SetVarying(const std::string_view p_name, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value);
		void SetVarying(const std::string_view p_name, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> p_value, uint8_t p_index);
		void SetSample(const std::string_view p_name, Resources::Texture* p_texture);

		template<typename T>
		constexpr T GetUniform(const std::string_view p_name) const { return std::get<T>(m_uniforms.at(p_name)); }
		template<typename T>
		constexpr T GetFlat(const std::string_view p_name) const { return std::get<T>(m_flats.at(p_name)); }
		template<typename T>
		constexpr T GetVarying(const std::string_view p_name) const { return std::get<T>(m_interpolatedVarying.at(p_name)); }

		Resources::Texture* GetSample(const std::string_view p_name) const { return m_samples.at(p_name); }

		template<typename T>
		constexpr void InterpolateData(const std::string_view p_key, const T& p_data1, const T& p_data2, const T& p_data3, const glm::vec3& p_barycentricCoords, float p_w0, float p_w1, float p_w2)
		{
			m_interpolatedVarying[p_key] =
				(p_data1 / p_w0) * p_barycentricCoords.z +
				(p_data2 / p_w1) * p_barycentricCoords.y +
				(p_data3 / p_w2) * p_barycentricCoords.x;
		}

	protected:
		virtual glm::vec4 VertexPass(const Geometry::Vertex & p_vertex) = 0;
		virtual Data::Color FragmentPass()                              = 0;

		glm::vec4 Texture(const Resources::Texture& p_texture, const glm::vec2& p_textCoords) const;

		float m_interpolatedReciprocal;

	private:
		uint8_t ComputeCurrentMipmapIndex(uint8_t p_mipmapsAmount) const;

	private:
		std::unordered_map<std::string_view, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>> m_uniforms;
		std::unordered_map<std::string_view, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>> m_flats;
		std::unordered_map<std::string_view, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>> m_varying[3];
		std::unordered_map<std::string_view, std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>> m_interpolatedVarying;

		std::unordered_map<std::string_view, Resources::Texture*> m_samples;

		uint8_t m_vertexIndex = 0;
	};
}
