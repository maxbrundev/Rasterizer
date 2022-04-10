#pragma once

#include "SDL.h"

#include "Geometry/Vertex.h"
#include "Geometry/Edge.h"
#include "Core/Renderer.h"

#include "Buffers/TextureBuffer.h"

namespace Core
{
	class Rasterizer
	{
	public:
		Rasterizer(Renderer& p_renderer, uint16_t p_textureBufferWidth, uint16_t p_textureBufferHeight);
		~Rasterizer() = default;

		void SetScissorRect(int p_minX, int p_minY, int p_maxX, int p_maxY);

		void DrawLine(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1) const;
		void DrawTriangle(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2) const;

		Buffers::TextureBuffer& GetTextureBuffer();

	private:
		Uint8 InterpolateColors(Uint8 p_color0, Uint8 p_color1, Uint8 p_color2, Geometry::Edge p_edge0, Geometry::Edge p_edge1, Geometry::Edge p_edge2, float p_area, float p_positionX, float p_positionY) const;

	private:
		Buffers::TextureBuffer m_textureBuffer;

		uint16_t m_minX = 0;
		uint16_t m_maxX = 0;
		uint16_t m_minY = 0;
		uint16_t m_maxY = 0;
	};
}