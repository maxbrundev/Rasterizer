#pragma once

#include "AmberGL/API/Export.h"

#include "AmberGL/SoftwareRenderer/Programs/AProgram.h"
#include "AmberGL/SoftwareRenderer/RenderObject/TextureObject.h"

#include "AmberGL/SoftwareRenderer/Defines.h"

namespace AmberGL
{
	API_AMBERGL void Initialize(uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight);
	API_AMBERGL void Terminate();
	API_AMBERGL void WindowHint(uint16_t p_name, int p_value);

	API_AMBERGL void ClearColor(float p_red, float p_green, float p_blue, float p_alpha);
	API_AMBERGL void Clear(uint8_t p_flags);
	API_AMBERGL void Viewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height);
	API_AMBERGL void DrawElements(uint16_t primitiveMode, uint32_t indexCount);
	API_AMBERGL void DrawArrays(uint16_t primitiveMode, uint32_t first, uint32_t count);
	API_AMBERGL void PointSize(float p_size);
	API_AMBERGL void LineWidth(float p_width);
	API_AMBERGL void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, const glm::vec4& p_color);
	API_AMBERGL void DrawPoint(const glm::vec3& p_point0);
	API_AMBERGL void DepthFunc(uint16_t p_func);

	API_AMBERGL uint32_t CreateProgram();
	API_AMBERGL void DeleteProgram(uint32_t p_program);
	API_AMBERGL void UseProgram(uint32_t p_program);
	API_AMBERGL void AttachShader(uint32_t p_program, AmberGL::SoftwareRenderer::Programs::AProgram* p_programInstance);
	API_AMBERGL void UseProgram(AmberGL::SoftwareRenderer::Programs::AProgram* p_programInstance);

	API_AMBERGL void SetSamples(uint8_t p_samples);
	API_AMBERGL void PolygonMode(uint16_t p_mode);
	API_AMBERGL void Enable(uint8_t p_state);
	API_AMBERGL void Disable(uint8_t p_state);
	API_AMBERGL bool IsEnabled(uint8_t p_capability);
	API_AMBERGL void CullFace(uint16_t p_face);
	API_AMBERGL void DepthMask(bool p_flag);
	API_AMBERGL void GetBool(uint16_t p_name, bool* p_params);
	API_AMBERGL void GetInt(uint16_t p_name, int* p_params);

	API_AMBERGL void GenVertexArrays(uint32_t p_count, uint32_t* p_arrays);
	API_AMBERGL void DeleteVertexArrays(uint32_t p_count, const uint32_t* p_arrays);
	API_AMBERGL void BindVertexArray(uint32_t p_array);

	API_AMBERGL void GenBuffers(uint32_t p_count, uint32_t* p_buffers);
	API_AMBERGL void DeleteBuffers(uint32_t p_count, const uint32_t* p_buffers);
	API_AMBERGL void BindBuffer(uint32_t p_target, uint32_t p_buffer);
	API_AMBERGL void BufferData(uint32_t p_target, size_t p_size, const void* p_data);

	API_AMBERGL void GenTextures(uint32_t p_count, uint32_t* p_textures);
	API_AMBERGL void DeleteTextures(uint32_t p_count, const uint32_t* p_textures);
	API_AMBERGL void BindTexture(uint32_t p_target, uint32_t p_texture);
	API_AMBERGL void TexImage2D(uint32_t p_target, uint32_t p_level, uint32_t p_internalFormat, uint32_t p_width, uint32_t p_height, uint32_t p_border, uint32_t p_format, uint32_t p_type, const void* p_data);
	API_AMBERGL void TexParameteri(uint32_t p_target, uint16_t p_pname, uint16_t p_param);
	API_AMBERGL void ActiveTexture(uint32_t p_unit);
	API_AMBERGL void GenerateMipmap(uint32_t p_target);
	API_AMBERGL AmberGL::SoftwareRenderer::RenderObject::TextureObject* GetTextureObject(uint32_t p_textureUnit);

	API_AMBERGL void GenFrameBuffers(uint32_t p_count, uint32_t* p_frameBuffers);
	API_AMBERGL void BindFrameBuffer(uint32_t p_target, uint32_t p_frameBuffer);
	API_AMBERGL void FrameBufferTexture2D(uint32_t p_target, uint16_t p_attachment, uint32_t p_textarget, uint32_t p_texture, int p_level);
	API_AMBERGL void DrawBuffer(uint32_t p_mode);
	API_AMBERGL void ReadBuffer(uint32_t p_mode);
	API_AMBERGL uint32_t* GetFrameBufferData();
	API_AMBERGL uint32_t GetFrameBufferRowSize();

	API_AMBERGL void BlitFrameBuffer(uint32_t p_sourceFrameBuffer, uint32_t p_destinationFrameBuffer, int p_sourceX0, int p_sourceY0, int p_sourceX1, int p_sourceY1, int p_destinationX0, int p_destinationY0, int p_destinationX1, int p_destinationY1, uint16_t p_mask, uint16_t p_filter);
}
