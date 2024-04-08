#include "Core/Rasterizer.h"

#include <algorithm>

#include "Geometry/Triangle.h"

Rendering::Rasterizer::Rasterizer(Context::Window& p_window, SDL_Renderer* p_sdlRenderer, uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight) :
m_window(p_window),
m_textureBuffer(p_sdlRenderer, p_rasterizationBufferWidth, p_rasterizationBufferHeight, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING),
m_depthBuffer(p_rasterizationBufferWidth, p_rasterizationBufferHeight)
{
	m_window.ResizeEvent.AddListener(std::bind(&Rasterizer::OnResize, this, std::placeholders::_1, std::placeholders::_2));
}

float Rendering::Rasterizer::ComputeEdge(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2) const
{
	return (p_vertex2.position.x - p_vertex0.position.x) * (p_vertex1.position.y - p_vertex0.position.y) - (p_vertex2.position.y - p_vertex0.position.y) * (p_vertex1.position.x - p_vertex0.position.x);
}


void Rendering::Rasterizer::ClearDepth()
{
	m_depthBuffer.Clear();
}

void Rendering::Rasterizer::RasterizeMesh(const Resources::Mesh& p_mesh, AShader& p_shader)
{
	const auto& vertices = p_mesh.GetVertices();
	const auto& indices  = p_mesh.GetIndices();

	if (!indices.empty())
	{
		for (uint32_t i = 0; i < indices.size(); i += 3)
		{
			RasterizeTriangle(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]], p_shader);
		}
	}
	else if (vertices.size() % 3 == 0)
	{
		for (uint32_t i = 0; i < vertices.size(); i += 3)
		{
			RasterizeTriangle(vertices[i], vertices[i + 1], vertices[i + 2], p_shader);
		}
	}
}

void Rendering::Rasterizer::RasterizeTriangle(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2, AShader& p_shader)
{
	glm::vec4 vertexWorldPosition0 = p_shader.ProcessVertex(p_vertex0, 0);
	glm::vec4 vertexWorldPosition1 = p_shader.ProcessVertex(p_vertex1, 1);
	glm::vec4 vertexWorldPosition2 = p_shader.ProcessVertex(p_vertex2, 2);

	glm::vec3 vertexScreenPosition0 = ComputeScreenSpaceCoordinate(vertexWorldPosition0);
	glm::vec3 vertexScreenPosition1 = ComputeScreenSpaceCoordinate(vertexWorldPosition1);
	glm::vec3 vertexScreenPosition2 = ComputeScreenSpaceCoordinate(vertexWorldPosition2);
	
	glm::vec2 vertexNormalizedPosition0 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition0);
	glm::vec2 vertexNormalizedPosition1 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition1);
	glm::vec2 vertexNormalizedPosition2 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition2);
	
	glm::vec2 vertexRasterPosition0 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition0);
	glm::vec2 vertexRasterPosition1 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition1);
	glm::vec2 vertexRasterPosition2 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition2);

	Geometry::Triangle triangle(vertexRasterPosition0, vertexRasterPosition1, vertexRasterPosition2);

	if ((m_state.CullFace == ECullFace::BACK && triangle.ComputeArea() >= 0.0f)
		|| (m_state.CullFace == ECullFace::FRONT && triangle.ComputeArea() <= 0.0f))
			return;

	const auto xMin = std::max(0, triangle.BoundingBox2D.Min.x);
	const auto yMin = std::max(0, triangle.BoundingBox2D.Min.y);

	const auto xMax = std::min(triangle.BoundingBox2D.Max.x, static_cast<int32_t>(m_textureBuffer.GetWidth()));
	const auto yMax = std::min(triangle.BoundingBox2D.Max.y, static_cast<int32_t>(m_textureBuffer.GetHeight()));

	for (int32_t x = xMin; x < xMax; x++)
	{
		for (int32_t y = yMin; y < yMax; y++)
		{
			const glm::vec3 barycentricCoords = triangle.GetBarycentricCoordinates({ x, y });

			if (barycentricCoords.x >= 0.0f && barycentricCoords.y >= 0.0f && barycentricCoords.x + barycentricCoords.y <= 1.0f)
			{
				const float depth = vertexScreenPosition0.z * barycentricCoords.z + vertexScreenPosition2.z * barycentricCoords.x + barycentricCoords.y * vertexScreenPosition1.z;

				if (!m_state.DepthTest || depth <= m_depthBuffer.GetElement(x, y))
				{
					p_shader.ProcessInterpolation(barycentricCoords, vertexWorldPosition0.w, vertexWorldPosition1.w, vertexWorldPosition2.w);

					Data::Color color = p_shader.ProcessFragment();

					m_textureBuffer.SetPixel(x, y, color);

					if (m_state.DepthWrite)
					{
						m_depthBuffer.SetElement(x, y, depth);
					}
				}
			}
		}
	}
}

void Rendering::Rasterizer::DrawPoint(const Geometry::Vertex& p_vertex0) const
{
	m_textureBuffer.SetPixel(p_vertex0.position.x, p_vertex0.position.y, p_vertex0.color);
}

void Rendering::Rasterizer::RasterizeLine(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, AShader& p_shader, const Data::Color& p_color)
{
	glm::vec4 vertexWorldPosition0 = p_shader.ProcessVertex(p_vertex0, 0);
	glm::vec4 vertexWorldPosition1 = p_shader.ProcessVertex(p_vertex1, 1);

	glm::vec3 vertexScreenPosition0 = ComputeScreenSpaceCoordinate(vertexWorldPosition0);
	glm::vec3 vertexScreenPosition1 = ComputeScreenSpaceCoordinate(vertexWorldPosition1);

	glm::vec2 vertexNormalizedPosition0 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition0);
	glm::vec2 vertexNormalizedPosition1 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition1);

	glm::vec2 vertexRasterPosition0 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition0);
	glm::vec2 vertexRasterPosition1 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition1);

	int x0 = static_cast<int>(vertexRasterPosition0.x);
	int y0 = static_cast<int>(vertexRasterPosition0.y);
	int x1 = static_cast<int>(vertexRasterPosition1.x);
	int y1 = static_cast<int>(vertexRasterPosition1.y);

	// Bresenham's line algorithm.
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx - dy;

	int width  = static_cast<int>(m_textureBuffer.GetWidth());
	int height = static_cast<int>(m_textureBuffer.GetHeight());

	float totalDistance = sqrt(dx * dx + dy * dy);

	while (x0 != x1 || y0 != y1)
	{
		float currentDistance = sqrt((x0 - vertexRasterPosition0.x) * (x0 - vertexRasterPosition0.x) + (y0 - vertexRasterPosition0.y) * (y0 - vertexRasterPosition0.y));

		float depth = vertexScreenPosition0.z * ((totalDistance - currentDistance) / totalDistance) + vertexScreenPosition1.z * (currentDistance / totalDistance);

		if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
		{
			if(!m_state.DepthTest || depth <= m_depthBuffer.GetElement(x0, y0))
			{
				m_textureBuffer.SetPixel(x0, y0, p_color);

				if(m_state.DepthWrite)
				{
					m_depthBuffer.SetElement(x0, y0, depth);
				}
			}
		}

		int e2 = (err << 1);

		if (e2 > -dy)
		{
			err -= dy;
			x0 += sx;
		}

		if (e2 < dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}

glm::vec3 Rendering::Rasterizer::ComputeScreenSpaceCoordinate(const glm::vec4& p_vertexWorldPosition)
{
	return p_vertexWorldPosition / p_vertexWorldPosition.w;
}

glm::vec2 Rendering::Rasterizer::ComputeNormalizedDeviceCoordinate(const glm::vec3& p_vertexScreenSpacePosition) const
{
	glm::vec2 normalizedCoordinate;

	normalizedCoordinate.x = (p_vertexScreenSpacePosition.x + 1.0f) * 0.5f;
	normalizedCoordinate.y = (1.0f - p_vertexScreenSpacePosition.y) * 0.5f;

	return normalizedCoordinate;
}

glm::vec2 Rendering::Rasterizer::ComputeRasterSpaceCoordinate(glm::vec2 p_vertexNormalizedPosition) const
{
	p_vertexNormalizedPosition.x = std::round(p_vertexNormalizedPosition.x * m_textureBuffer.GetWidth());
	p_vertexNormalizedPosition.y = std::round(p_vertexNormalizedPosition.y * m_textureBuffer.GetHeight());

	return p_vertexNormalizedPosition;
}

glm::vec3 Rendering::Rasterizer::GetBarycentricWeights(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2, const Geometry::Vertex& p_point)
{
	float areaABC = ComputeEdge(p_vertex0, p_vertex1, p_vertex2);
	float areaPBC = ComputeEdge(p_point, p_vertex1, p_vertex2);
	float areaPCA = ComputeEdge(p_vertex0, p_point, p_vertex2);

	float alpha = areaPBC / areaABC;
	float beta = areaPCA / areaABC;
	float gamma = 1.0f - alpha - beta;

	return glm::vec3(alpha, beta, gamma);
}

Data::Color Rendering::Rasterizer::InterpolateColors(const Data::Color& c0, const Data::Color& c1, const Data::Color& c2,
                                                const glm::vec3& weights)
{
	float r = weights.x * c0.r + weights.y * c1.r + weights.z * c2.r;
	float g = weights.x * c0.g + weights.y * c1.g + weights.z * c2.g;
	float b = weights.x * c0.b + weights.y * c1.b + weights.z * c2.b;

	return Data::Color(r, g, b);
}

Buffers::TextureBuffer& Rendering::Rasterizer::GetTextureBuffer()
{
	return m_textureBuffer;
}

Rendering::RenderState& Rendering::Rasterizer::GetRenderState()
{
	return m_state;
}

void Rendering::Rasterizer::Clear(const Data::Color& p_color)
{
	m_textureBuffer.Clear(p_color);
}

void Rendering::Rasterizer::SendDataToGPU()
{
	m_textureBuffer.SendDataToGPU();
}

void Rendering::Rasterizer::OnResize(uint16_t p_width, uint16_t p_height)
{
	//m_textureBuffer.Resize(p_width, p_height);
	//m_depthBuffer.Resize(p_width, p_height);
}
