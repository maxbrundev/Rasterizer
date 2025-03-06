#pragma once

#include <glm/glm.hpp>

#include "AmberRenderer/Rendering/Rasterizer/Shaders/EShaderUniformType.h"

namespace AmberRenderer::Rendering
{
	template <typename T>
	struct ShaderTypeTraits
	{
		static constexpr EShaderDataType Type = EShaderDataType::INT;
		static constexpr uint8_t TypeCount = 0;

		static void WriteToBuffer(const T& p_value, float* p_buffer)
		{
			static_assert(sizeof(T) == 0, "No specialization for this type.");
		}

		static T ReadFromBuffer(const float* p_buffer)
		{
			static_assert(sizeof(T) == 0, "No specialization for this type.");
			return T{};
		}
	};

	template <>
	struct ShaderTypeTraits<int>
	{
		static constexpr EShaderDataType Type = EShaderDataType::INT;
		static constexpr uint8_t TypeCount = 1;

		static void WriteToBuffer(const int& p_value, float* p_buffer)
		{
			p_buffer[0] = static_cast<float>(p_value);
		}

		static int ReadFromBuffer(const float* p_buffer)
		{
			return static_cast<int>(p_buffer[0]);
		}
	};

	template <>
	struct ShaderTypeTraits<float>
	{
		static constexpr EShaderDataType Type = EShaderDataType::FLOAT;
		static constexpr uint8_t TypeCount = 1;

		static void WriteToBuffer(const float& p_value, float* p_buffer)
		{
			p_buffer[0] = p_value;
		}

		static float ReadFromBuffer(const float* p_buffer)
		{
			return p_buffer[0];
		}
	};

	template <>
	struct ShaderTypeTraits<glm::vec2>
	{
		static constexpr EShaderDataType Type = EShaderDataType::VEC2;
		static constexpr uint8_t TypeCount = 2;

		static void WriteToBuffer(const glm::vec2& p_value, float* p_buffer)
		{
			p_buffer[0] = p_value.x;
			p_buffer[1] = p_value.y;
		}

		static glm::vec2 ReadFromBuffer(const float* p_buffer)
		{
			return glm::vec2(p_buffer[0], p_buffer[1]);
		}
	};

	template <>
	struct ShaderTypeTraits<glm::vec3>
	{
		static constexpr EShaderDataType Type = EShaderDataType::VEC3;
		static constexpr uint8_t TypeCount = 3;

		static void WriteToBuffer(const glm::vec3& p_value, float* p_buffer)
		{
			p_buffer[0] = p_value.x;
			p_buffer[1] = p_value.y;
			p_buffer[2] = p_value.z;
		}

		static glm::vec3 ReadFromBuffer(const float* p_buffer)
		{
			return glm::vec3(p_buffer[0], p_buffer[1], p_buffer[2]);
		}
	};

	template <>
	struct ShaderTypeTraits<glm::vec4>
	{
		static constexpr EShaderDataType Type = EShaderDataType::VEC4;
		static constexpr uint8_t TypeCount = 4;

		static void WriteToBuffer(const glm::vec4& p_value, float* p_buffer)
		{
			p_buffer[0] = p_value.x;
			p_buffer[1] = p_value.y;
			p_buffer[2] = p_value.z;
			p_buffer[3] = p_value.w;
		}

		static glm::vec4 ReadFromBuffer(const float* p_buffer)
		{
			return glm::vec4(p_buffer[0], p_buffer[1], p_buffer[2], p_buffer[3]);
		}
	};

	template <>
	struct ShaderTypeTraits<glm::mat2>
	{
		static constexpr EShaderDataType Type = EShaderDataType::MAT2;
		static constexpr uint8_t TypeCount = 4;

		static void WriteToBuffer(const glm::mat2& p_value, float* p_buffer)
		{
			p_buffer[0] = p_value[0][0];
			p_buffer[1] = p_value[0][1];
			p_buffer[2] = p_value[1][0];
			p_buffer[3] = p_value[1][1];
		}

		static glm::mat2 ReadFromBuffer(const float* p_buffer)
		{
			return glm::mat2(
				p_buffer[0], p_buffer[1],
				p_buffer[2], p_buffer[3]
			);
		}
	};

	template <>
	struct ShaderTypeTraits<glm::mat3>
	{
		static constexpr EShaderDataType Type = EShaderDataType::MAT3;
		static constexpr uint8_t TypeCount = 9;

		static void WriteToBuffer(const glm::mat3& p_value, float* p_buffer)
		{
			p_buffer[0] = p_value[0][0];
			p_buffer[1] = p_value[0][1];
			p_buffer[2] = p_value[0][2];
			p_buffer[3] = p_value[1][0];
			p_buffer[4] = p_value[1][1];
			p_buffer[5] = p_value[1][2];
			p_buffer[6] = p_value[2][0];
			p_buffer[7] = p_value[2][1];
			p_buffer[8] = p_value[2][2];
		}

		static glm::mat3 ReadFromBuffer(const float* p_buffer)
		{
			return glm::mat3(
				p_buffer[0], p_buffer[1], p_buffer[2],
				p_buffer[3], p_buffer[4], p_buffer[5],
				p_buffer[6], p_buffer[7], p_buffer[8]
			);
		}
	};

	template <>
	struct ShaderTypeTraits<glm::mat4>
	{
		static constexpr EShaderDataType Type = EShaderDataType::MAT4;
		static constexpr uint8_t TypeCount = 16;

		static void WriteToBuffer(const glm::mat4& p_value, float* p_buffer)
		{
			p_buffer[0] = p_value[0][0];
			p_buffer[1] = p_value[0][1];
			p_buffer[2] = p_value[0][2];
			p_buffer[3] = p_value[0][3];
			p_buffer[4] = p_value[1][0];
			p_buffer[5] = p_value[1][1];
			p_buffer[6] = p_value[1][2];
			p_buffer[7] = p_value[1][3];
			p_buffer[8] = p_value[2][0];
			p_buffer[9] = p_value[2][1];
			p_buffer[10] = p_value[2][2];
			p_buffer[11] = p_value[2][3];
			p_buffer[12] = p_value[3][0];
			p_buffer[13] = p_value[3][1];
			p_buffer[14] = p_value[3][2];
			p_buffer[15] = p_value[3][3];
		}

		static glm::mat4 ReadFromBuffer(const float* p_buffer)
		{
			return glm::mat4(
				p_buffer[0], p_buffer[1], p_buffer[2], p_buffer[3],
				p_buffer[4], p_buffer[5], p_buffer[6], p_buffer[7],
				p_buffer[8], p_buffer[9], p_buffer[10], p_buffer[11],
				p_buffer[12], p_buffer[13], p_buffer[14], p_buffer[15]
			);
		}
	};
}
