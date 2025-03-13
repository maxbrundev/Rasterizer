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

#define GLR_TEXTURE_2D           0
#define GLR_DEPTH_COMPONENT      0x1902
#define GLR_RGBA8                 0x1908
#define GLR_FLOAT                0x1406
#define GLR_UNSIGNED_BYTE        0x1401
#define GLR_TEXTURE_MIN_FILTER   0x2801
#define GLR_TEXTURE_MAG_FILTER   0x2800
#define GLR_TEXTURE_WRAP_S       0x2802
#define GLR_TEXTURE_WRAP_T       0x2803

#define GLR_CLAMP 0
#define GLR_REPEAT 1

#define GLR_NEAREST 0
#define GLR_LINEAR 1
#define GLR_NEAREST_MIPMAP_NEAREST 2
#define GLR_LINEAR_MIPMAP_LINEAR 3
#define GLR_LINEAR_MIPMAP_NEAREST 4
#define GLR_NEAREST_MIPMAP_LINEAR 5

#define GL_TEXTURE0 0

#define GLR_FRAMEBUFFER       0x8D40
#define GLR_COLOR_ATTACHMENT  0x8CE0
#define GLR_DEPTH_ATTACHMENT  0x8D00
#define GL_NONE    0

struct TextureObject
{
	uint32_t ID;
	uint32_t Target;
	uint32_t InternalFormat;
	uint32_t Width;
	uint32_t Height;

	union
	{
		uint8_t* Data8;
		//TODO: Investigate the necessary of a 32 bit buffer.
		//float* Data32;
	};

	uint8_t MinFilter;
	uint8_t MagFilter;
	uint8_t WrapS;
	uint8_t WrapT;
};

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

	void GenTextures(uint32_t p_count, uint32_t* p_textures);
	void DeleteTextures(uint32_t p_count, const uint32_t* p_textures);
	void BindTexture(uint32_t p_target, uint32_t p_texture);
	void TexImage2D(uint32_t p_target, uint32_t p_level, uint32_t p_internalFormat, uint32_t p_width, uint32_t p_height, uint32_t p_border, uint32_t p_format, uint32_t p_type, const void* p_data);
	void TexParameteri(uint32_t p_target, uint32_t p_pname, uint8_t p_param);
	void ActiveTexture(uint32_t p_unit);
	TextureObject* GetTextureObject(uint32_t textureUnit);

	void GenFramebuffers(uint32_t count, uint32_t* framebuffers);
	void BindFramebuffer(uint32_t target, uint32_t framebuffer);
	void FramebufferTexture2D(uint32_t target, uint32_t attachment, uint32_t textarget, uint32_t texture, int level);
	void DrawBuffer(uint32_t mode);
	void ReadBuffer(uint32_t mode);
}
