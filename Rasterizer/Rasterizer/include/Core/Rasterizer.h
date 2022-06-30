#pragma once

#include "SDL.h"

#include "Geometry/Vertex.h"
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
		void DrawWireFrameTriangle(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2) const;

		void DrawTriangle(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2);

		Buffers::TextureBuffer& GetTextureBuffer();

	private:
		float ComputeEdge(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_point);

	private:
		Buffers::TextureBuffer m_textureBuffer;

		uint16_t m_minX = 0;
		uint16_t m_maxX = 0;
		uint16_t m_minY = 0;
		uint16_t m_maxY = 0;
	};
}