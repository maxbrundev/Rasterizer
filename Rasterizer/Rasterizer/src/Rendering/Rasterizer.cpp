#include "Rendering/Rasterizer.h"

#include <algorithm>
#include <array>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Geometry/Triangle.h"

Rendering::Rasterizer::Rasterizer(Context::Window& p_window, SDL_Renderer* p_sdlRenderer, uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight) :
m_window(p_window),
m_textureBuffer(p_sdlRenderer, p_rasterizationBufferWidth, p_rasterizationBufferHeight, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING),
m_msaaBuffer(p_rasterizationBufferWidth, p_rasterizationBufferHeight),
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

void Rendering::Rasterizer::NormalizePlane(Geometry::Plane& p_plane)
{
	float magnitude = glm::length(p_plane.Normal);
	if (magnitude > 0.0f) {  // Check to avoid division by zero
		p_plane.Normal /= magnitude;
		p_plane.Distance /= magnitude;
	}
}

void Rendering::Rasterizer::RasterizeMesh(const Resources::Mesh& p_mesh, AShader& p_shader)
{
	float aspect_x = static_cast<float>(m_window.GetSize().first) / static_cast<float>(m_window.GetSize().second);
	float fov_y = glm::radians(60.0f);
	float fov_x = atan(tan(fov_y / 2) * aspect_x) * 2;
	float z_near = 0.1f;
	float z_far = 100.0f;

	float cos_half_fov_x = cos(fov_x / 2.0f);
	float sin_half_fov_x = sin(fov_x / 2.0f);
	float cos_half_fov_y = cos(fov_y / 2.0f);
	float sin_half_fov_y = sin(fov_y / 2.0f);

	m_clippingFrustum[0].Distance = glm::vec3(0, 0, 0);
	m_clippingFrustum[0].Normal.x = cos_half_fov_x;
	m_clippingFrustum[0].Normal.y = 0;
	m_clippingFrustum[0].Normal.z = sin_half_fov_x;

	m_clippingFrustum[1].Distance = glm::vec3(0, 0, 0);
	m_clippingFrustum[1].Normal.x = -cos_half_fov_x;
	m_clippingFrustum[1].Normal.y = 0;
	m_clippingFrustum[1].Normal.z = sin_half_fov_x;

	m_clippingFrustum[2].Distance = glm::vec3(0, 0, 0);
	m_clippingFrustum[2].Normal.x = 0;
	m_clippingFrustum[2].Normal.y = -cos_half_fov_y;
	m_clippingFrustum[2].Normal.z = sin_half_fov_y;

	m_clippingFrustum[3].Distance = glm::vec3(0, 0, 0);
	m_clippingFrustum[3].Normal.x = 0;
	m_clippingFrustum[3].Normal.y = cos_half_fov_y;
	m_clippingFrustum[3].Normal.z = sin_half_fov_y;

	m_clippingFrustum[4].Distance = glm::vec3(0, 0, z_near);
	m_clippingFrustum[4].Normal.x = 0;
	m_clippingFrustum[4].Normal.y = 0;
	m_clippingFrustum[4].Normal.z = 1;

	m_clippingFrustum[5].Distance = glm::vec3(0, 0, z_far);
	m_clippingFrustum[5].Normal.x = 0;
	m_clippingFrustum[5].Normal.y = 0;
	m_clippingFrustum[5].Normal.z = -1;

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

	if(m_samples > 0)
	{
		const int32_t width  = static_cast<int32_t>(m_textureBuffer.GetWidth());
		const int32_t height = static_cast<int32_t>(m_textureBuffer.GetHeight());

		for (int32_t x = 0; x < width; x++)
		{
			for (int32_t y = 0; y < height; y++)
			{
				auto& samples = m_msaaBuffer.Data[y * m_textureBuffer.GetWidth() + x];
		
				uint16_t count = 0;

				glm::ivec4 color(0);
		
				float depth = 0.0f;
		
				for (const auto sample : samples)
				{
					const uint8_t a = sample.first;
					const uint8_t b = sample.first >> 8;
					const uint8_t g = sample.first >> 16;
					const uint8_t r = sample.first >> 24;

					color.x += r;
					color.y += g;
					color.z += b;
					color.w += a;
		
					depth += sample.second;
		
					count++;
				}
		
				color.x /= count;
				color.y /= count;
				color.z /= count;
				color.w /= count;

				depth /= static_cast<float>(count);

				Data::Color sampledColorTotal(static_cast<uint8_t>(color.x), static_cast<uint8_t>(color.y), static_cast<uint8_t>(color.z), static_cast<uint8_t>(color.w));
				const float alpha = static_cast<float>(sampledColorTotal.a) / 255.0f;
		
				m_textureBuffer.SetPixel(x, y, Data::Color::Mix(Data::Color(m_textureBuffer.GetPixel(x, y)), sampledColorTotal, alpha));
		
				if (m_state.DepthWrite)
				{
					m_depthBuffer.SetElement(x, y, depth);
				}
			}
		}
	}
}



void Rendering::Rasterizer::RasterizeTriangle(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2, AShader& p_shader)
{
	std::array<glm::vec4, 3> verticesWorldPosition{ p_shader.ProcessVertex(p_vertex0, 0), p_shader.ProcessVertex(p_vertex1, 1) , p_shader.ProcessVertex(p_vertex2, 2) };

	Polygon currentPoly;
	currentPoly.Vertices = { verticesWorldPosition[0], verticesWorldPosition[1], verticesWorldPosition[2] };
	currentPoly.textCoords = { p_vertex0.textCoords, p_vertex1.textCoords, p_vertex2.textCoords };
	currentPoly.VerticesCount = 3;

	for (const auto& plane : m_clippingFrustum)
	{
		ClipAgainstPlane(currentPoly, plane);
	}

	struct TriangleClip
	{
		glm::vec4 Points[3];
		glm::vec2 TextCoords[3];
	};

	TriangleClip test[12];

	for (int i = 0; i < currentPoly.VerticesCount - 2; i++) {

		int index0 = 0;
		int index1 = i + 1;
		int index2 = i + 2;

		test[i].Points[0] = currentPoly.Vertices[index0];
		test[i].Points[1] = currentPoly.Vertices[index1];
		test[i].Points[2] = currentPoly.Vertices[index2];

		test[i].TextCoords[0] = currentPoly.textCoords[index0];
		test[i].TextCoords[1] = currentPoly.textCoords[index1];
		test[i].TextCoords[2] = currentPoly.textCoords[index2];
	}

	for (int i = 0; i < currentPoly.VerticesCount - 2; i++)
	{
		glm::vec4 position0;
		glm::vec4 position1;
		glm::vec4 position2;

		glm::vec2 textCoords0;
		glm::vec2 textCoords1;
		glm::vec2 textCoords2;

		position0 = test[i].Points[0];
		textCoords0 = test[i].TextCoords[0];
		
		position1 = test[i].Points[1];
		textCoords1 = test[i].TextCoords[1];
		
		position2 = test[i].Points[2];
		textCoords2 = test[i].TextCoords[2];

		std::array<glm::vec4, 3> processVertices{ position0, position1, position2 };

		RasterizeLine(position0, position1, Data::Color::Red);
		RasterizeLine(position1, position2, Data::Color::Red);
		RasterizeLine(position2, position0, Data::Color::Red);

		p_shader.SetVarying("v_TextCoords", textCoords0, 0);
		p_shader.SetVarying("v_TextCoords", textCoords1, 1);
		p_shader.SetVarying("v_TextCoords", textCoords2, 2);

		glm::vec3 vertexScreenPosition0 = ComputeScreenSpaceCoordinate(processVertices[0]);
		glm::vec3 vertexScreenPosition1 = ComputeScreenSpaceCoordinate(processVertices[1]);
		glm::vec3 vertexScreenPosition2 = ComputeScreenSpaceCoordinate(processVertices[2]);
		
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
		
		auto xMin = std::max(0, triangle.BoundingBox2D.Min.x);
		auto yMin = std::max(0, triangle.BoundingBox2D.Min.y);
		
		auto xMax = std::min(triangle.BoundingBox2D.Max.x, static_cast<int32_t>(m_textureBuffer.GetWidth()));
		auto yMax = std::min(triangle.BoundingBox2D.Max.y, static_cast<int32_t>(m_textureBuffer.GetHeight()));
		
		if (m_samples > 0)
		{
			for (int32_t x = xMin; x < xMax; x++)
			{
				for (int32_t y = yMin; y < yMax; y++)
				{
					for (uint8_t sampleIndex = 0; sampleIndex < m_samples; sampleIndex++)
					{
						float samplePosX = static_cast<float>(x) + (static_cast<float>(sampleIndex) + 0.5f) / static_cast<float>(m_samples);
						float samplePosY = static_cast<float>(y) + (static_cast<float>(sampleIndex) + 0.5f) / static_cast<float>(m_samples);
		
						const glm::vec3 barycentricCoords = triangle.GetBarycentricCoordinates({ samplePosX, samplePosY });
		
						if (barycentricCoords.x >= 0.0f && barycentricCoords.y >= 0.0f && barycentricCoords.x + barycentricCoords.y <= 1.0f)
						{
							const float depth = vertexScreenPosition0.z * barycentricCoords.z + vertexScreenPosition2.z * barycentricCoords.x + barycentricCoords.y * vertexScreenPosition1.z;
		
							if (!m_state.DepthTest || depth <= m_depthBuffer.GetElement(x, y))
							{
								p_shader.ProcessInterpolation(barycentricCoords, processVertices[0].w, processVertices[1].w, processVertices[2].w);
		
								Data::Color color = p_shader.ProcessFragment();
		
								m_msaaBuffer.SetPixelSample(x, y, sampleIndex, color, depth);
							}
						}
					}
				}
			}
		}
		else
		{
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
							p_shader.ProcessInterpolation(barycentricCoords, processVertices[0].w, processVertices[1].w, processVertices[2].w);
		
							Data::Color color = p_shader.ProcessFragment();
		
							float alpha = color.a / 255.0f;
		
							m_textureBuffer.SetPixel(x, y, Data::Color::Mix(Data::Color(m_textureBuffer.GetPixel(x, y)), color, alpha));
		
							if (m_state.DepthWrite)
							{
								m_depthBuffer.SetElement(x, y, depth);
							}
						}
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

void Rendering::Rasterizer::RasterizeLine(const glm::vec4& p_vertex0, const glm::vec4& p_vertex1,
	const Data::Color& p_color)
{
	glm::vec3 vertexScreenPosition0 = ComputeScreenSpaceCoordinate(p_vertex0);
	glm::vec3 vertexScreenPosition1 = ComputeScreenSpaceCoordinate(p_vertex1);

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

	int width = static_cast<int>(m_textureBuffer.GetWidth());
	int height = static_cast<int>(m_textureBuffer.GetHeight());

	float totalDistance = sqrt(dx * dx + dy * dy);

	while (x0 != x1 || y0 != y1)
	{
		float currentDistance = sqrt((x0 - vertexRasterPosition0.x) * (x0 - vertexRasterPosition0.x) + (y0 - vertexRasterPosition0.y) * (y0 - vertexRasterPosition0.y));

		float depth = vertexScreenPosition0.z * ((totalDistance - currentDistance) / totalDistance) + vertexScreenPosition1.z * (currentDistance / totalDistance);

		if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
		{
			if (!m_state.DepthTest || depth <= m_depthBuffer.GetElement(x0, y0))
			{
				m_textureBuffer.SetPixel(x0, y0, p_color);

				if (m_state.DepthWrite)
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

Data::Color Rendering::Rasterizer::InterpolateColors(const Data::Color& c0, const Data::Color& c1, const Data::Color& c2, const glm::vec3& weights)
{
	float r = weights.x * c0.r + weights.y * c1.r + weights.z * c2.r;
	float g = weights.x * c0.g + weights.y * c1.g + weights.z * c2.g;
	float b = weights.x * c0.b + weights.y * c1.b + weights.z * c2.b;

	return Data::Color(r, g, b);
}

void Rendering::Rasterizer::ClipAgainstPlane(Polygon& p_polygon, const Geometry::Plane& p_plane)
{
	glm::vec4 insideVertices[12];
	glm::vec2 insideTextCoords[12];
	uint8_t insideVerticesCount = 0;

	glm::vec4 previousVertex     = p_polygon.Vertices[p_polygon.VerticesCount > 0 ? p_polygon.VerticesCount - 1 : 0];
	glm::vec2 previousTextCoords = p_polygon.textCoords[p_polygon.VerticesCount > 0 ? p_polygon.VerticesCount - 1 : 0];

	float previousDotValue = glm::dot(glm::vec3(previousVertex) - p_plane.Distance, p_plane.Normal);

	for(int i = 0; i < p_polygon.VerticesCount; i++)
	{
		float currentDotValue = glm::dot(glm::vec3(p_polygon.Vertices[i]) - p_plane.Distance, p_plane.Normal);

		if (currentDotValue * previousDotValue < 0.0f)
		{
			float t = previousDotValue / (previousDotValue - currentDotValue);

			glm::vec4 intersectionPoint = 
			{
				glm::lerp(previousVertex.x, p_polygon.Vertices[i].x, t),
				glm::lerp(previousVertex.y, p_polygon.Vertices[i].y, t),
				glm::lerp(previousVertex.z, p_polygon.Vertices[i].z, t),
				glm::lerp(previousVertex.w, p_polygon.Vertices[i].w, t)
			};

			glm::vec2 interpolatedTextCoords = 
			{
				glm::lerp(previousTextCoords.x, p_polygon.textCoords[i].x, t),
				glm::lerp(previousTextCoords.y, p_polygon.textCoords[i].y, t)
			};

			insideVertices[insideVerticesCount]   = intersectionPoint;
			insideTextCoords[insideVerticesCount] = interpolatedTextCoords;
			insideVerticesCount++;
		}

		if (currentDotValue > 0.0f) 
		{
			insideVertices[insideVerticesCount]   = p_polygon.Vertices[i];
			insideTextCoords[insideVerticesCount] = p_polygon.textCoords[i];
			insideVerticesCount++;
		}

		previousDotValue  = currentDotValue;
		previousVertex    = p_polygon.Vertices[i];
		previousTextCoords = p_polygon.textCoords[i];
	}

	for (int i = 0; i < insideVerticesCount; i++) 
	{
		p_polygon.Vertices[i]   = insideVertices[i];
		p_polygon.textCoords[i] = insideTextCoords[i];
	}
	
	p_polygon.VerticesCount = insideVerticesCount;
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
	m_msaaBuffer.Clear(p_color);
}

void Rendering::Rasterizer::SendDataToGPU()
{
	m_textureBuffer.SendDataToGPU();
}

void Rendering::Rasterizer::SetSamples(uint8_t p_samples)
{
	m_samples = p_samples;
	m_msaaBuffer.SetSamplesAmount(p_samples);
}

void Rendering::Rasterizer::OnResize(uint16_t p_width, uint16_t p_height)
{
	//m_textureBuffer.Resize(p_width, p_height);
	//m_depthBuffer.Resize(p_width, p_height);
}
