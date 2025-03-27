#pragma once

#include <cstdint>

#include "AmberRenderer/Rendering/Rasterizer/Shaders/AShader.h"
#include "AmberRenderer/Data/Color.h"

constexpr bool CLIPPING = true;

typedef AmberRenderer::Data::Color RGBA8;
typedef float Depth;

// -----------------------------------------------------------------------------
// PRIMITIVE TYPES
// -----------------------------------------------------------------------------
#define GLR_TRIANGLES      0
#define GLR_LINES          1
#define GLR_POINTS         2
#define GLR_TRIANGLE_STRIP 3

// -----------------------------------------------------------------------------
// POLYGON MODES
// -----------------------------------------------------------------------------
#define GLR_FILL  0
#define GLR_LINE  1
#define GLR_POINT 2

// -----------------------------------------------------------------------------
// FACE-CULLING MODES
// -----------------------------------------------------------------------------
#define GLR_BACK           0
#define GLR_FRONT          1
#define GLR_FRONT_AND_BACK 2

// -----------------------------------------------------------------------------
// STATE ENABLE/DISABLE
// -----------------------------------------------------------------------------
#define GLR_DEPTH_WRITE 0x01
#define GLR_DEPTH_TEST  0x02
#define GLR_CULL_FACE   0x04
#define GLR_MULTISAMPLE 0x08

#define GLR_SAMPLES 0

// -----------------------------------------------------------------------------
// CLEAR FLAGS
// -----------------------------------------------------------------------------
#define GLR_COLOR_BUFFER_BIT 0x01
#define GLR_DEPTH_BUFFER_BIT 0x02

// -----------------------------------------------------------------------------
// BUFFER BINDING TARGETS
// -----------------------------------------------------------------------------
#define GLR_ARRAY_BUFFER         0
#define GLR_ELEMENT_ARRAY_BUFFER 1

// -----------------------------------------------------------------------------
// TEXTURE TARGETS
// -----------------------------------------------------------------------------
#define GLR_TEXTURE_2D 100

// -----------------------------------------------------------------------------
// INTERNAL / PIXEL FORMATS
// -----------------------------------------------------------------------------
#define GLR_DEPTH_COMPONENT 101
#define GLR_RGBA8           102
#define GLR_FLOAT           103
#define GLR_UNSIGNED_BYTE   104

// -----------------------------------------------------------------------------
// TEXTURE PARAMETERS
// -----------------------------------------------------------------------------
#define GLR_TEXTURE_MIN_FILTER 110
#define GLR_TEXTURE_MAG_FILTER 111
#define GLR_TEXTURE_WRAP_S     112
#define GLR_TEXTURE_WRAP_T     113

// -----------------------------------------------------------------------------
// WRAP MODES
// -----------------------------------------------------------------------------
#define GLR_CLAMP  0
#define GLR_REPEAT 1

// -----------------------------------------------------------------------------
// FILTERING MODES
// -----------------------------------------------------------------------------
#define GLR_NEAREST                0
#define GLR_LINEAR                 1
#define GLR_NEAREST_MIPMAP_NEAREST 2
#define GLR_LINEAR_MIPMAP_LINEAR   3
#define GLR_LINEAR_MIPMAP_NEAREST  4
#define GLR_NEAREST_MIPMAP_LINEAR  5

// -----------------------------------------------------------------------------
// TEXTURE UNIT
// -----------------------------------------------------------------------------
#define GLR_TEXTURE0 0

// -----------------------------------------------------------------------------
// FRAMEBUFFER ATTACHMENTS
// -----------------------------------------------------------------------------
#define GLR_FRAMEBUFFER      200
#define GLR_COLOR_ATTACHMENT 201
#define GLR_DEPTH_ATTACHMENT 202

// -----------------------------------------------------------------------------
// NO ATTACHMENT
// -----------------------------------------------------------------------------
#define GL_NONE 999

// -----------------------------------------------------------------------------
// CUSTOM DEFINES
// -----------------------------------------------------------------------------
#define GLR_VIEW_PORT 300

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

	uint8_t** Mipmaps;
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
	void GenerateMipmap(uint32_t p_target);
	TextureObject* GetTextureObject(uint32_t p_textureUnit);

	void GenFramebuffers(uint32_t p_count, uint32_t* p_framebuffers);
	void BindFramebuffer(uint32_t p_target, uint32_t p_framebuffer);
	void FramebufferTexture2D(uint32_t p_target, uint32_t p_attachment, uint32_t p_textarget, uint32_t p_texture, int p_level);
	void DrawBuffer(uint32_t p_mode);
	void ReadBuffer(uint32_t p_mode);
	uint32_t* GetFrameBufferData();
	uint32_t GetFrameBufferRowSize();
}
