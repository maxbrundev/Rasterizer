#pragma once

#include <cstdint>

#include "Buffers/TextureBuffer.h"
#include "Context/Window.h"
#include "Resources/Model.h"

#define GLR_TRIANGLES 0
#define GLR_LINES     1
#define GLR_POINTS    2

#define GLR_BACK  0x10
#define GLR_FRONT 0x20

#define GLR_DEPTH_WRITE 0x01
#define GLR_DEPTH_TEST  0x02
#define GLR_CULL_FACE   0x04

namespace GLRasterizer
{
	void MakeCurrentContext(Context::Window* p_window, SDL_Renderer* p_sdlRenderer, uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight);

	void Clear(const Data::Color& p_color);
	void ClearDepth();
	void DrawElements(uint8_t  p_drawMode, const Resources::Mesh& p_mesh);
	void DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, const Data::Color& p_color);
	void SendDataToGPU();
	void UseProgram(Rendering::AShader* p_shader);
	void SetState(uint8_t p_state);
	void SetSamples(uint8_t p_samples);

	void Enable(uint8_t p_state);
	void Disable(uint8_t p_state);
	void CullFace(uint8_t p_face);
	void DepthMask(bool p_flag);

	bool GetBool(int p_parameter);
	bool GetCapability(int p_capability);
	int  GetInt(int p_parameter);
	Buffers::TextureBuffer& GetTextureBuffer();
}
