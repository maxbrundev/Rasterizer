#pragma once

#include <cstdint>

#include "AmberRenderer/Data/Color.h"
#include "AmberRenderer/Rendering/SoftwareRenderer/Defines.h"
#include "AmberRenderer/Rendering/SoftwareRenderer/RenderObject/TextureObject.h"
#include "AmberRenderer/Rendering/SoftwareRenderer/Programs/AProgram.h"

namespace AmberGL
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

	uint32_t CreateProgram();
	void DeleteProgram(uint32_t p_program);
	void UseProgram(uint32_t p_program);
	void AttachShader(uint32_t p_program, AmberRenderer::Rendering::SoftwareRenderer::Programs::AProgram* p_programInstance);
	void UseProgram(AmberRenderer::Rendering::SoftwareRenderer::Programs::AProgram* p_programInstance);

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
	AmberRenderer::Rendering::SoftwareRenderer::RenderObject::TextureObject* GetTextureObject(uint32_t p_textureUnit);

	void GenFramebuffers(uint32_t p_count, uint32_t* p_framebuffers);
	void BindFramebuffer(uint32_t p_target, uint32_t p_framebuffer);
	void FramebufferTexture2D(uint32_t p_target, uint32_t p_attachment, uint32_t p_textarget, uint32_t p_texture, int p_level);
	void DrawBuffer(uint32_t p_mode);
	void ReadBuffer(uint32_t p_mode);
	uint32_t* GetFrameBufferData();
	uint32_t GetFrameBufferRowSize();
}
