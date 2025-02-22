#pragma once

#include <cstdint>

#include "Buffers/TextureBuffer.h"
#include "Context/Window.h"
#include "Resources/Model.h"

constexpr bool CLIPPING = true;

#define GLR_TRIANGLES 0
#define GLR_LINES  1
#define GLR_POINTS 2

#define GLR_FILL  0
#define GLR_LINE  1
#define GLR_POINT 2

#define GLR_BACK            0
#define GLR_FRONT           1
#define GLR_FRONT_AND_BACK  2

#define GLR_DEPTH_WRITE 0x01
#define GLR_DEPTH_TEST  0x02
#define GLR_CULL_FACE   0x04

namespace GLRasterizer
{
	void Initialize(uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight);

	void Clear(const Data::Color& p_color);
	void ClearDepth();
	void DrawElements(uint8_t  p_drawMode, const Resources::Mesh& p_mesh);
	void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, const Data::Color& p_color);
	void UseProgram(Rendering::AShader* p_shader);
	void SetSamples(uint8_t p_samples);
	void PolygoneMode(uint8_t p_mode);
	void Enable(uint8_t p_state);
	void Disable(uint8_t p_state);
	bool IsEnabled(uint8_t p_capability);
	void CullFace(uint8_t p_face);
	void DepthMask(bool p_flag);

	void GetBool(uint8_t p_name, bool* p_params);
	void GetInt(uint8_t p_name, int* p_params);
	void Terminate();
	Buffers::TextureBuffer* GetFrameBuffer();
	uint32_t* GetFrameBufferDate();
	Buffers::TextureBuffer& GetTextureBuffer();
}
