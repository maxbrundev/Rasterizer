#include "Core/Rasterizer.h"

#include <algorithm>

Core::Rasterizer::Rasterizer(Renderer& p_renderer, uint16_t p_textureBufferWidth, uint16_t p_textureBufferHeight) :
	m_textureBuffer(p_renderer.m_sdlRenderer, p_textureBufferWidth, p_textureBufferHeight, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING)
{
}

void Core::Rasterizer::SetScissorRect(int p_minX, int p_minY, int p_maxX, int p_maxY)
{
	m_minX = p_minX;
	m_minY = p_minY;
	m_maxX = p_maxX;
	m_maxY = p_maxY;
}

void Core::Rasterizer::DrawLine(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1) const
{
	const float xdiff = (p_vertex1.x - p_vertex0.x);
	const float ydiff = (p_vertex1.y - p_vertex0.y);

	if (xdiff == 0.0f && ydiff == 0.0f)
	{
		m_textureBuffer.SetPixel(p_vertex0.x, p_vertex0.y, p_vertex0.color);
		return;
	}

	if (fabs(xdiff) > fabs(ydiff))
	{
		uint16_t xmin;
		uint16_t xmax;

		if (p_vertex0.x < p_vertex1.x) 
		{
			xmin = p_vertex0.x;
			xmax = p_vertex1.x;
		}
		else
		{
			xmin = p_vertex1.x;
			xmax = p_vertex0.x;
		}

		const float slope = ydiff / xdiff;

		for (uint16_t x = xmin; x <= xmax; x++)
		{
			const uint16_t y = p_vertex0.y + (x - p_vertex0.x) * slope;

			const Uint8 red   = p_vertex0.color.r + (p_vertex1.color.r - p_vertex0.color.r) * ((x - p_vertex0.x) / xdiff);
			const Uint8 green = p_vertex0.color.g + (p_vertex1.color.g - p_vertex0.color.g) * ((x - p_vertex0.x) / xdiff);
			const Uint8 blue  = p_vertex0.color.b + (p_vertex1.color.b - p_vertex0.color.b) * ((x - p_vertex0.x) / xdiff);

			Data::Color color(red, green, blue);

			m_textureBuffer.SetPixel(x, y, color);
		}
	}
	else
	{
		uint16_t ymin;
		uint16_t ymax;

		if (p_vertex0.y < p_vertex1.y)
		{
			ymin = p_vertex0.y;
			ymax = p_vertex1.y;
		}
		else
		{
			ymin = p_vertex1.y;
			ymax = p_vertex0.y;
		}

		const float slope = xdiff / ydiff;

		for (uint16_t y = ymin; y <= ymax; y++)
		{
			const uint16_t x = p_vertex0.x + (y - p_vertex0.y) * slope;

			const Uint8 red   = p_vertex0.color.r + (p_vertex1.color.r - p_vertex0.color.r) * ((y - p_vertex0.y) / ydiff);
			const Uint8 green = p_vertex0.color.g + (p_vertex1.color.g - p_vertex0.color.g) * ((y - p_vertex0.y) / ydiff);
			const Uint8 blue  = p_vertex0.color.b + (p_vertex1.color.b - p_vertex0.color.b) * ((y - p_vertex0.y) / ydiff);

			Data::Color color(red, green, blue);

			m_textureBuffer.SetPixel(x, y, color);
		}
	}
}

void Core::Rasterizer::DrawWireFrameTriangle(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2) const
{
	DrawLine(p_vertex0, p_vertex1);
	DrawLine(p_vertex1, p_vertex2);
	DrawLine(p_vertex2, p_vertex0);
}

float Core::Rasterizer::ComputeEdge(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2) const
{
	return (p_vertex2.x - p_vertex0.x) * (p_vertex1.y - p_vertex0.y) - (p_vertex2.y - p_vertex0.y) * (p_vertex1.x - p_vertex0.x);
}

void Core::Rasterizer::DrawTriangle(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1,const Geometry::Vertex& p_vertex2) const
{
	uint16_t minX = std::min(std::min(p_vertex0.x, p_vertex1.x), p_vertex2.x);
	uint16_t maxX = std::max(std::max(p_vertex0.x, p_vertex1.x), p_vertex2.x);
	uint16_t minY = std::min(std::min(p_vertex0.y, p_vertex1.y), p_vertex2.y);
	uint16_t maxY = std::max(std::max(p_vertex0.y, p_vertex1.y), p_vertex2.y);

	minX = std::max(minX, m_minX);
	maxX = std::min(maxX, m_maxX);
	minY = std::max(minY, m_minY);
	maxY = std::min(maxY, m_maxY);

	//Area of the triangle
	const float area = ComputeEdge(p_vertex0, p_vertex1, p_vertex2);

	for (uint16_t x = minX; x <= maxX; x++)
	{
		for (uint16_t y = minY; y <= maxY; y++)
		{
			Geometry::Vertex point = { x + 0.5f, y + 0.5f };

			// Compute Barycentric Coordinates
			float w0 = ComputeEdge(p_vertex1, p_vertex2, point);
			float w1 = ComputeEdge(p_vertex2, p_vertex0, point);
			float w2 = ComputeEdge(p_vertex0, p_vertex1, point);

			if (w0 >= 0 && w1 >= 0 && w2 >= 0)
			{
				w0 /= area;
				w1 /= area;
				w2 /= area;

				// Interpolate colors using Barycentric Coordinates
				const float red   = w0 * p_vertex0.color.r + w1 * p_vertex1.color.r + w2 * p_vertex2.color.r;
				const float green = w0 * p_vertex0.color.g + w1 * p_vertex1.color.g + w2 * p_vertex2.color.g;
				const float blue  = w0 * p_vertex0.color.b + w1 * p_vertex1.color.b + w2 * p_vertex2.color.b;

				Data::Color color(red, green, blue);

				m_textureBuffer.SetPixel(x, y, color);
			}
		}
	}
}

void Core::Rasterizer::DrawPoint(const Geometry::Vertex& p_vertex0) const
{
	m_textureBuffer.SetPixel(p_vertex0.x, p_vertex0.y, p_vertex0.color);
}

Buffers::TextureBuffer& Core::Rasterizer::GetTextureBuffer()
{
	return m_textureBuffer;
}
