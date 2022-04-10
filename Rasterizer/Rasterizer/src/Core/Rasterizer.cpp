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

Uint8 Core::Rasterizer::InterpolateColors(Uint8 p_color0, Uint8 p_color1, Uint8 p_color2, Geometry::Edge p_edge0, Geometry::Edge p_edge1, Geometry::Edge p_edge2, float p_area, float p_positionX, float p_positionY) const
{
	float factor = 1.0f / (2.0f * p_area);

	const float a = factor * (p_color0 * p_edge0.a + p_color1 * p_edge1.a + p_color2 * p_edge2.a);
	const float b = factor * (p_color0 * p_edge0.b + p_color1 * p_edge1.b + p_color2 * p_edge2.b);
	const float c = factor * (p_color0 * p_edge0.c + p_color1 * p_edge1.c + p_color2 * p_edge2.c);

	return a * p_positionX + b * p_positionY + c;
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

		const uint16_t slope = ydiff / xdiff;

		for (uint16_t x = xmin; x <= xmax; x++)
		{
			const uint16_t y = p_vertex0.y + ((x - p_vertex0.x) * slope);

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

		const uint16_t slope = xdiff / ydiff;

		for (uint16_t y = ymin; y <= ymax; y++)
		{
			const uint16_t x = p_vertex0.x + ((y - p_vertex0.y) * slope);

			const Uint8 red   = p_vertex0.color.r + (p_vertex1.color.r - p_vertex0.color.r) * ((y - p_vertex0.y) / ydiff);
			const Uint8 green = p_vertex0.color.g + (p_vertex1.color.g - p_vertex0.color.g) * ((y - p_vertex0.y) / ydiff);
			const Uint8 blue  = p_vertex0.color.b + (p_vertex1.color.b - p_vertex0.color.b) * ((y - p_vertex0.y) / ydiff);

			Data::Color color(red, green, blue);

			m_textureBuffer.SetPixel(x, y, color);
		}
	}
}

void Core::Rasterizer::DrawTriangle(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2) const
{
	uint16_t minX = std::min(std::min(p_vertex0.x, p_vertex1.x), p_vertex2.x);
	uint16_t maxX = std::max(std::max(p_vertex0.x, p_vertex1.x), p_vertex2.x);
	uint16_t minY = std::min(std::min(p_vertex0.y, p_vertex1.y), p_vertex2.y);
	uint16_t maxY = std::max(std::max(p_vertex0.y, p_vertex1.y), p_vertex2.y);

	minX = std::max(minX, m_minX);
	maxX = std::min(maxX, m_maxX);
	minY = std::max(minY, m_minY);
	maxY = std::min(maxY, m_maxY);

	const Geometry::Edge edge0(p_vertex1, p_vertex2);
	const Geometry::Edge edge1(p_vertex2, p_vertex0);
	const Geometry::Edge edge2(p_vertex0, p_vertex1);

	const float area = 0.5f * (edge0.c + edge1.c + edge2.c);

	if (area < 0)
		return;

	for (uint16_t x = minX; x <= maxX; x++)
	{
		for (uint16_t y = minY; y <= maxY; y++)
		{
			if (edge0.IsInsideEdge(x, y) && edge1.IsInsideEdge(x, y) && edge2.IsInsideEdge(x, y))
			{
				const Uint8 red   = InterpolateColors(p_vertex0.color.r, p_vertex1.color.r, p_vertex2.color.r, edge0, edge1, edge2, area, x, y);
				const Uint8 green = InterpolateColors(p_vertex0.color.g, p_vertex1.color.g, p_vertex2.color.g, edge0, edge1, edge2, area, x, y);
				const Uint8 blue  = InterpolateColors(p_vertex0.color.b, p_vertex1.color.b, p_vertex2.color.b, edge0, edge1, edge2, area, x, y);

				Data::Color color(red, green, blue);

				m_textureBuffer.SetPixel(x, y, color);
			}
		}
	}
}

Buffers::TextureBuffer& Core::Rasterizer::GetTextureBuffer()
{
	return m_textureBuffer;
}
