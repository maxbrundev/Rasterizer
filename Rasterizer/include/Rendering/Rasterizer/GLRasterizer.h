#pragma once

#include <cstdint>

#include "Rendering/Rasterizer/Buffers/TextureBuffer.h"
#include "Rendering/Rasterizer/Shaders/AShader.h"

constexpr bool CLIPPING = true;

#define GLR_TRIANGLES 0
#define GLR_LINES     1
#define GLR_POINTS    2

#define GLR_FILL  0
#define GLR_LINE  1
#define GLR_POINT 2

#define GLR_BACK           0
#define GLR_FRONT          1
#define GLR_FRONT_AND_BACK 2

#define GLR_DEPTH_WRITE 0x01
#define GLR_DEPTH_TEST  0x02
#define GLR_CULL_FACE   0x04

#define GLR_ARRAY_BUFFER         0
#define GLR_ELEMENT_ARRAY_BUFFER 1

namespace GLRasterizer
{
	void Initialize(uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight);
	void Terminate();

	void Clear(const Data::Color& p_color);
	void ClearDepth();
	
	void DrawElements(uint8_t primitiveMode, uint32_t indexCount);
	void DrawArrays(uint8_t primitiveMode, uint32_t first, uint32_t count);
	void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, const Data::Color& p_color);

	void UseProgram(Rendering::Rasterizer::Shaders::AShader* p_shader);
	void SetSamples(uint8_t p_samples);
	void PolygonMode(uint8_t p_mode);
	void Enable(uint8_t p_state);
	void Disable(uint8_t p_state);
	bool IsEnabled(uint8_t p_capability);
	void CullFace(uint8_t p_face);
	void DepthMask(bool p_flag);
	void GetBool(uint8_t p_name, bool* p_params);
	void GetInt(uint8_t p_name, int* p_params);

	Rendering::Rasterizer::Buffers::TextureBuffer* GetFrameBuffer();
	uint32_t* GetFrameBufferDate();
	Rendering::Rasterizer::Buffers::TextureBuffer& GetTextureBuffer();

	void GenVertexArrays(uint32_t p_count, uint32_t* p_arrays);
	void DeleteVertexArrays(uint32_t p_count, const uint32_t* p_arrays);
	void BindVertexArray(uint32_t p_array);

	void GenBuffers(uint32_t p_count, uint32_t* p_buffers);
	void DeleteBuffers(uint32_t p_count, const uint32_t* p_buffers);
	void BindBuffer(uint32_t p_target, uint32_t p_buffer);
	void BufferData(uint32_t p_target, size_t p_size, const void* p_data);
}
