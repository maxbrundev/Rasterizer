#pragma once

#include <cstdint>

#include "AmberRenderer/Rendering/Rasterizer/Buffers/FrameBuffer.h"
#include "AmberRenderer/Rendering/Rasterizer/Shaders/AShader.h"

constexpr bool CLIPPING = true;

#define GLR_TRIANGLES 0
#define GLR_LINES     1
#define GLR_POINTS    2
#define GLR_TRIANGLE_STRIP 3

#define GLR_FILL  0
#define GLR_LINE  1
#define GLR_POINT 2

#define GLR_BACK           0
#define GLR_FRONT          1
#define GLR_FRONT_AND_BACK 2

#define GLR_DEPTH_WRITE 0x01
#define GLR_DEPTH_TEST  0x02
#define GLR_CULL_FACE   0x04
#define GLR_MULTISAMPLE 0x08

#define GLR_ARRAY_BUFFER         0
#define GLR_ELEMENT_ARRAY_BUFFER 1

#define GLR_COLOR_BUFFER_BIT 0x01
#define GLR_DEPTH_BUFFER_BIT 0x02

#define GLR_SAMPLES 0x10

namespace GLRasterizer
{
	void Initialize(uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight);
	void Terminate();

	void WindowHint(uint8_t p_name, uint8_t p_value);

	void ClearColor(float p_red, float p_green, float p_blue, float p_alpha);
	void Clear(uint8_t p_flags);
	void Viewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height);
	void DrawElements(uint8_t primitiveMode, uint32_t indexCount);
	void DrawArrays(uint8_t primitiveMode, uint32_t first, uint32_t count);
	void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, const AmberRenderer::Data::Color& p_color);

	void UseProgram(AmberRenderer::Rendering::Rasterizer::Shaders::AShader* p_shader);
	void SetSamples(uint8_t p_samples);
	void PolygonMode(uint8_t p_mode);
	void Enable(uint8_t p_state);
	void Disable(uint8_t p_state);
	bool IsEnabled(uint8_t p_capability);
	void CullFace(uint8_t p_face);
	void DepthMask(bool p_flag);
	void GetBool(uint8_t p_name, bool* p_params);
	void GetInt(uint8_t p_name, int* p_params);

	AmberRenderer::Rendering::Rasterizer::Buffers::FrameBuffer<RGBA8>* GetFrameBuffer();
	AmberRenderer::Rendering::Rasterizer::Buffers::FrameBuffer<Depth>* GetDepthBuffer();
	uint32_t* GetFrameBufferDate();
	void SetActiveBuffers(AmberRenderer::Rendering::Rasterizer::Buffers::FrameBuffer<RGBA8>* p_color, AmberRenderer::Rendering::Rasterizer::Buffers::FrameBuffer<Depth>* p_depth);
	void GenVertexArrays(uint32_t p_count, uint32_t* p_arrays);
	void DeleteVertexArrays(uint32_t p_count, const uint32_t* p_arrays);
	void BindVertexArray(uint32_t p_array);

	void GenBuffers(uint32_t p_count, uint32_t* p_buffers);
	void DeleteBuffers(uint32_t p_count, const uint32_t* p_buffers);
	void BindBuffer(uint32_t p_target, uint32_t p_buffer);
	void BufferData(uint32_t p_target, size_t p_size, const void* p_data);
}
