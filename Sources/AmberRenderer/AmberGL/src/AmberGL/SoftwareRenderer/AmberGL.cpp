#include "AmberGL/SoftwareRenderer/AmberGL.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <algorithm>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

#include "AmberGL/Geometry/Line.h"
#include "AmberGL/Geometry/Plane.h"
#include "AmberGL/Geometry/Point.h"
#include "AmberGL/Geometry/Polygon.h"
#include "AmberGL/Geometry/Triangle.h"
#include "AmberGL/SoftwareRenderer/Defines.h"
#include "AmberGL/SoftwareRenderer/TextureSampler.h"

#include "AmberGL/SoftwareRenderer/Buffers/MSAABuffer.h"
#include "AmberGL/SoftwareRenderer/RenderObject/ProgramObject.h"
#include "AmberGL/SoftwareRenderer/RenderObject/RenderContext.h"
#include "AmberGL/SoftwareRenderer/RenderObject/VertexArrayObject.h"
#include "AmberGL/SoftwareRenderer/RenderObject/VertexBufferObject.h"

namespace
{
	AmberGL::SoftwareRenderer::RenderObject::RenderContext RenderContext;

	std::unordered_map<uint32_t, AmberGL::SoftwareRenderer::RenderObject::VertexArrayObject> VertexArrayObjects;
	std::unordered_map<uint32_t, AmberGL::SoftwareRenderer::RenderObject::BufferObject> BufferObjects;

	uint32_t VertexArrayID = 1;
	uint32_t CurrentVertexArrayObject = 0;

	uint32_t BufferID = 1;
	uint32_t CurrentArrayBuffer = 0;
	uint32_t CurrentElementBuffer = 0;

	AmberGL::SoftwareRenderer::RenderObject::VertexArrayObject* GetBoundVertexArrayObject()
	{
		if (CurrentVertexArrayObject == 0)
			return nullptr;

		auto it = VertexArrayObjects.find(CurrentVertexArrayObject);

		return (it != VertexArrayObjects.end()) ? &it->second : nullptr;
	}

	std::unordered_map<uint32_t, AmberGL::SoftwareRenderer::RenderObject::TextureObject*> TextureObjects;
	uint32_t TextureID = 1;
	uint32_t CurrentTexture = 0;

	constexpr uint32_t MAX_TEXTURE_UNITS = 16;
	uint32_t CurrentActiveTextureUnit = 0;
	AmberGL::SoftwareRenderer::RenderObject::TextureObject* BoundTextureUnits[MAX_TEXTURE_UNITS] = { nullptr };

	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject BackBuffer;

	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<RGBA8>* ActiveColorBuffer = nullptr;
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Depth>* ActiveDepthBuffer = nullptr;

	std::unordered_map<uint32_t, AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject> FrameBufferObjects;
	uint32_t FrameBufferObjectID = 1;
	uint32_t CurrentFrameBuffer = 0;

	std::unordered_map<uint32_t, AmberGL::SoftwareRenderer::RenderObject::ProgramObject> ProgramObjects;
	uint32_t ProgramID = 1;
	uint32_t CurrentProgramObject = 0;
}

void InitializeClippingFrustum();

glm::vec3 ComputeScreenSpaceCoordinate(const glm::vec4& p_vertexWorldPosition);
glm::vec2 ComputeNormalizedDeviceCoordinate(const glm::vec3& p_vertexScreenSpacePosition);
glm::vec2 ComputeRasterSpaceCoordinate(glm::vec2 p_vertexNormalizedPosition);

void RasterizeTriangle(const AmberGL::Geometry::Vertex& p_vertex0, const AmberGL::Geometry::Vertex& p_vertex1, const AmberGL::Geometry::Vertex& p_vertex2);

void TransformAndRasterizeVertices(const std::array<glm::vec4, 3>& processedVertices);

void ComputeFragments(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices);
void SetFragment(const AmberGL::Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec2& p_dfdx, const glm::vec2& p_dfdy);
void SetSampleFragment(const AmberGL::Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, float p_sampleX, float p_sampleY, uint8_t p_sampleIndex, const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec2& p_dfdx, const glm::vec2& p_dfdy);

void ClipAgainstPlane(AmberGL::Geometry::Polygon& p_polygon, const AmberGL::Geometry::Plane& p_plane);

void ProcessPoint(const AmberGL::Geometry::Vertex& p_vertex);
void RasterizePoint(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec4& p_point);
void RasterizePoint(const AmberGL::Geometry::Point& p_point);

void ProcessLine(const AmberGL::Geometry::Vertex& p_vertex0, const AmberGL::Geometry::Vertex& p_vertex1);
void RasterizeLineDirectly(const std::array<glm::vec4, 2>& p_transformedVertices);
void RasterizeLine(const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_start, const glm::vec4& p_end);

void RasterizeTriangleWireframe(const std::array<glm::vec4, 3>& transformedVertices);
void RasterizeTrianglePoints(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices);

void ApplyMSAA();

bool DepthTest(float fragmentDepth, float bufferDepth);

//TODO: One MSAA per framebuffer
AmberGL::SoftwareRenderer::Buffers::MSAABuffer* MSAABuffer;
std::array<AmberGL::Geometry::Plane, 6> ClippingFrustum;

inline uint32_t PackColor(const glm::vec4& p_normalizedColor)
{
	return static_cast<uint8_t>(p_normalizedColor.x * 255.0f) << 24 |
		static_cast<uint8_t>(p_normalizedColor.y * 255.0f) << 16 |
		static_cast<uint8_t>(p_normalizedColor.z * 255.0f) << 8 |
		static_cast<uint8_t>(p_normalizedColor.w * 255.0f);
}

inline glm::vec4 UnpackColor(uint32_t p_packedColor)
{
	return glm::vec4(
		(p_packedColor >> 24 & 0xFF) / 255.0f,
		(p_packedColor >> 16 & 0xFF) / 255.0f,
		(p_packedColor >> 8 & 0xFF) / 255.0f,
		(p_packedColor & 0xFF) / 255.0f
	);
}

void InitializeClippingFrustum()
{
	ClippingFrustum[0].Distance = 1.0f;
	ClippingFrustum[0].Normal.x = 1.0f;
	ClippingFrustum[0].Normal.y = 0.0f;
	ClippingFrustum[0].Normal.z = 0.0f;

	ClippingFrustum[1].Distance = 1.0f;
	ClippingFrustum[1].Normal.x = -1.0f;
	ClippingFrustum[1].Normal.y = 0.0f;
	ClippingFrustum[1].Normal.z = 0.0f;

	ClippingFrustum[2].Distance = 1.0f;
	ClippingFrustum[2].Normal.x = 0.0f;
	ClippingFrustum[2].Normal.y = 1.0f;
	ClippingFrustum[2].Normal.z = 0.0f;

	ClippingFrustum[3].Distance = 1.0f;
	ClippingFrustum[3].Normal.x = 0.0f;
	ClippingFrustum[3].Normal.y = -1.0f;
	ClippingFrustum[3].Normal.z = 0.0f;

	ClippingFrustum[4].Distance = 1.0f;
	ClippingFrustum[4].Normal.x = 0.0f;
	ClippingFrustum[4].Normal.y = 0.0f;
	ClippingFrustum[4].Normal.z = 1.0f;

	ClippingFrustum[5].Distance = 1.0f;
	ClippingFrustum[5].Normal.x = 0.0f;
	ClippingFrustum[5].Normal.y = 0.0f;
	ClippingFrustum[5].Normal.z = -1.0f;
}

glm::vec3 ComputeScreenSpaceCoordinate(const glm::vec4& p_vertexWorldPosition)
{
	return p_vertexWorldPosition / p_vertexWorldPosition.w;
}

glm::vec2 ComputeNormalizedDeviceCoordinate(const glm::vec3& p_vertexScreenSpacePosition)
{
	glm::vec2 normalizedCoordinate;

	normalizedCoordinate.x = (p_vertexScreenSpacePosition.x + 1.0f) * 0.5f;
	normalizedCoordinate.y = (1.0f - p_vertexScreenSpacePosition.y) * 0.5f;

	return normalizedCoordinate;
}

glm::vec2 ComputeRasterSpaceCoordinate(glm::vec2 p_vertexNormalizedPosition)
{
	p_vertexNormalizedPosition.x = RenderContext.Viewport.X + p_vertexNormalizedPosition.x * RenderContext.Viewport.Width - 0.5f;
	p_vertexNormalizedPosition.y = RenderContext.Viewport.Y + p_vertexNormalizedPosition.y * RenderContext.Viewport.Height - 0.5f;

	return p_vertexNormalizedPosition;
}

void RasterizeTriangle(const AmberGL::Geometry::Vertex& p_vertex0, const AmberGL::Geometry::Vertex& p_vertex1, const AmberGL::Geometry::Vertex& p_vertex2)
{
	std::array<glm::vec4, 3> processedVertices
	{
		RenderContext.Program->ProcessVertex(p_vertex0, 0),
		RenderContext.Program->ProcessVertex(p_vertex1, 1),
		RenderContext.Program->ProcessVertex(p_vertex2, 2)
	};

	AmberGL::Geometry::Polygon currentPoly;
	currentPoly.Vertices = { processedVertices[0], processedVertices[1], processedVertices[2] };
	currentPoly.TexCoords = { p_vertex0.texCoords, p_vertex1.texCoords, p_vertex2.texCoords };
	currentPoly.Normals = { p_vertex0.normal, p_vertex1.normal, p_vertex2.normal };
	currentPoly.VerticesCount = 3;

	auto& programVaryings = RenderContext.Program->GetVaryings();

	int totalSize = 0;

	for (const auto& [name, varying] : programVaryings)
	{
		totalSize += RenderContext.Program->GetTypeCount(varying.Type);
	}

	currentPoly.VaryingsDataSize = totalSize;

	uint16_t offset = 0;

	for (auto& [name, varying] : programVaryings)
	{
		uint8_t typeCount = RenderContext.Program->GetTypeCount(varying.Type);

		for (uint8_t vertIdx = 0; vertIdx < 3; vertIdx++)
		{
			for (uint8_t i = 0; i < typeCount; i++)
			{
				currentPoly.Varyings[vertIdx][offset + i] = varying.Data[vertIdx][i];
			}
		}

		offset += typeCount;
	}

	for (const auto& plane : ClippingFrustum)
	{
		ClipAgainstPlane(currentPoly, plane);
	}

	for (uint8_t i = 0; i < currentPoly.VerticesCount - 2; i++)
	{
		const std::array<glm::vec4, 3> clippedVertices
		{
			currentPoly.Vertices[0],
			currentPoly.Vertices[i + 1],
			currentPoly.Vertices[i + 2]
		};

		offset = 0;
		for (auto& [name, varying] : programVaryings)
		{
			uint8_t typeCount = RenderContext.Program->GetTypeCount(varying.Type);

			for (uint8_t j = 0; j < typeCount; j++)
			{
				varying.Data[0][j] = currentPoly.Varyings[0][offset + j];
				varying.Data[1][j] = currentPoly.Varyings[i + 1][offset + j];
				varying.Data[2][j] = currentPoly.Varyings[i + 2][offset + j];
			}

			offset += typeCount;
		}

		TransformAndRasterizeVertices(clippedVertices);
	}
}

void TransformAndRasterizeVertices(const std::array<glm::vec4, 3>& processedVertices)
{
	glm::vec3 vertexScreenPosition0 = ComputeScreenSpaceCoordinate(processedVertices[0]);
	glm::vec3 vertexScreenPosition1 = ComputeScreenSpaceCoordinate(processedVertices[1]);
	glm::vec3 vertexScreenPosition2 = ComputeScreenSpaceCoordinate(processedVertices[2]);

	glm::vec2 vertexNormalizedPosition0 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition0);
	glm::vec2 vertexNormalizedPosition1 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition1);
	glm::vec2 vertexNormalizedPosition2 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition2);

	glm::vec2 vertexRasterPosition0 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition0);
	glm::vec2 vertexRasterPosition1 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition1);
	glm::vec2 vertexRasterPosition2 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition2);

	std::array<glm::vec4, 3> transformedVertices{ processedVertices[0], processedVertices[1], processedVertices[2] };

	transformedVertices[0].x = vertexRasterPosition0.x;
	transformedVertices[0].y = vertexRasterPosition0.y;
	transformedVertices[0].z = vertexScreenPosition0.z;

	transformedVertices[1].x = vertexRasterPosition1.x;
	transformedVertices[1].y = vertexRasterPosition1.y;
	transformedVertices[1].z = vertexScreenPosition1.z;

	transformedVertices[2].x = vertexRasterPosition2.x;
	transformedVertices[2].y = vertexRasterPosition2.y;
	transformedVertices[2].z = vertexScreenPosition2.z;

	AmberGL::Geometry::Triangle triangle(vertexRasterPosition0, vertexRasterPosition1, vertexRasterPosition2);

	float area = triangle.ComputeArea();

	if (RenderContext.State & AGL_CULL_FACE)
	{
		if ((RenderContext.CullFace == AGL_BACK && area > 0.0f) 
			|| (RenderContext.CullFace == AGL_FRONT && area < 0.0f) 
			|| RenderContext.CullFace == AGL_FRONT_AND_BACK)
			return;
	}

	switch (RenderContext.PolygonMode)
	{
	case AGL_FILL:
		ComputeFragments(triangle, transformedVertices);
		break;
	case AGL_LINE:
		RasterizeTriangleWireframe(transformedVertices);
		break;
	case AGL_POINT:
		RasterizeTrianglePoints(triangle, transformedVertices);
		break;
	}
}

void ComputeFragments(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices)
{
	const int xMin = std::max(p_triangle.BoundingBox2D.Min.x, 0);
	const int yMin = std::max(p_triangle.BoundingBox2D.Min.y, 0);
	const int xMax = std::min(p_triangle.BoundingBox2D.Max.x, static_cast<int32_t>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Width : RenderContext.FrameBufferObject->DepthBuffer->Width));
	const int yMax = std::min(p_triangle.BoundingBox2D.Max.y, static_cast<int32_t>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Height : RenderContext.FrameBufferObject->DepthBuffer->Height));

	if (xMax <= xMin || yMax <= yMin)
		return;

	for (uint32_t y = yMin; y < yMax; y += 2)
	{
		for (uint32_t x = xMin; x < xMax; x += 2)
		{
			glm::vec2 texCoords[2][2];

			for (int dy = 0; dy < 2; dy++)
			{
				for (int dx = 0; dx < 2; dx++)
				{
					uint32_t px = x + dx;
					uint32_t py = y + dy;

					if (px >= xMax || py >= yMax)
						continue;

					const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ px, py });

					RenderContext.Program->ProcessInterpolation(barycentricCoords, transformedVertices[0].w, transformedVertices[1].w, transformedVertices[2].w);

					texCoords[dy][dx] = RenderContext.Program->GetVaryingAs<glm::vec2>("v_TexCoords");
				}
			}

			for (int dy = 0; dy < 2; ++dy)
			{
				for (int dx = 0; dx < 2; ++dx)
				{
					uint32_t px = x + dx;
					uint32_t py = y + dy;

					if (px >= xMax || py >= yMax)
						continue;

					glm::vec2 dfdx = texCoords[dy][1] - texCoords[dy][0];
					glm::vec2 dfdy = texCoords[1][dx] - texCoords[0][dx];

					if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer != nullptr)
					{
						uint8_t gridSize = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));

						uint8_t sampleCount = 0;

						for (uint8_t j = 0; j < gridSize; j++)
						{
							for (uint8_t i = 0; i < gridSize; i++)
							{
								if (sampleCount >= RenderContext.Samples)
									break;

								const float samplePosX = px + (i + 0.5f) / gridSize;
								const float samplePosY = py + (j + 0.5f) / gridSize;

								SetSampleFragment(p_triangle, px, py, samplePosX, samplePosY, sampleCount, transformedVertices, dfdx, dfdy);
								sampleCount++;
							}
						}
					}
					else
					{
						SetFragment(p_triangle, px, py, transformedVertices, dfdx, dfdy);
					}
				}
			}
		}
	}
}

void SetFragment(const AmberGL::Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec2& p_dfdx, const glm::vec2& p_dfdy)
{
	const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ p_x, p_y });

	if (barycentricCoords.x >= 0.0f && barycentricCoords.y >= 0.0f && barycentricCoords.x + barycentricCoords.y <= 1.0f)
	{
		float depth = p_transformedVertices[0].z * barycentricCoords.x
			+ p_transformedVertices[1].z * barycentricCoords.y
			+ p_transformedVertices[2].z * barycentricCoords.z;

		depth = depth * 0.5f + 0.5f;

		if (depth < 0.0f || depth > 1.0f)
			return;

		if (!(RenderContext.State & AGL_DEPTH_TEST) || DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(p_x, p_y)))
		{
			RenderContext.Program->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

			RenderContext.Program->SetDerivatives(p_dfdx, p_dfdy);

			const glm::vec4 color = RenderContext.Program->ProcessFragment();

			if (RenderContext.FrameBufferObject->ColorBuffer != nullptr)
			{
				RenderContext.FrameBufferObject->ColorBuffer->SetPixel(p_x, p_y, PackColor(glm::mix(UnpackColor(RenderContext.FrameBufferObject->ColorBuffer->GetPixel(p_x, p_y)), color, color.w)));
			}

			if (RenderContext.State & AGL_DEPTH_WRITE)
			{
				RenderContext.FrameBufferObject->DepthBuffer->SetPixel(p_x, p_y, depth);
			}
		}
	}
}

void SetSampleFragment(const AmberGL::Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, float p_sampleX, float p_sampleY, uint8_t p_sampleIndex, const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec2& p_dfdx, const glm::vec2& p_dfdy)
{
	const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ p_sampleX, p_sampleY });

	if (barycentricCoords.x >= 0.0f && barycentricCoords.y >= 0.0f && barycentricCoords.x + barycentricCoords.y <= 1.0f)
	{
		float depth = p_transformedVertices[0].z * barycentricCoords.x + p_transformedVertices[1].z * barycentricCoords.y + p_transformedVertices[2].z * barycentricCoords.z;

		depth = depth * 0.5f + 0.5f;

		if (depth < 0.0f || depth > 1.0f)
			return;

		if (!(RenderContext.State & AGL_DEPTH_TEST) || DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(p_x, p_y)))
		{
			RenderContext.Program->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

			RenderContext.Program->SetDerivatives(p_dfdx, p_dfdy);

			const glm::vec4 color = RenderContext.Program->ProcessFragment();

			MSAABuffer->SetPixelSample(p_x, p_y, p_sampleIndex, PackColor(color), depth);
		}
	}
}

void ClipAgainstPlane(AmberGL::Geometry::Polygon& p_polygon, const AmberGL::Geometry::Plane& p_plane)
{
	if (p_polygon.VerticesCount == 0)
		return;

	const glm::vec4 plane4D = glm::vec4(
		p_plane.Normal.x,
		p_plane.Normal.y,
		p_plane.Normal.z,
		p_plane.Distance
	);

	glm::vec4 insideVertices[AmberGL::Geometry::MAX_POLY_COUNT];
	glm::vec2 insideTexCoords[AmberGL::Geometry::MAX_POLY_COUNT];
	glm::vec3 insideNormals[AmberGL::Geometry::MAX_POLY_COUNT];
	float insideVaryings[AmberGL::Geometry::MAX_POLY_COUNT][16];

	uint8_t insideVerticesCount = 0;

	glm::vec4 previousVertex = p_polygon.Vertices[p_polygon.VerticesCount - 1];
	glm::vec2 previousTexCoords = p_polygon.TexCoords[p_polygon.VerticesCount - 1];
	glm::vec3 previousNormal = p_polygon.Normals[p_polygon.VerticesCount - 1];
	float* previousVarying = p_polygon.Varyings[p_polygon.VerticesCount - 1];

	float previousDotValue = glm::dot(previousVertex, plane4D);

	for (int i = 0; i < p_polygon.VerticesCount; i++)
	{
		float currentDotValue = glm::dot(p_polygon.Vertices[i], plane4D);

		if (previousDotValue * currentDotValue < 0.0f)
		{
			float t = previousDotValue / (previousDotValue - currentDotValue);

			insideVertices[insideVerticesCount] = {
				glm::lerp(previousVertex.x, p_polygon.Vertices[i].x, t),
				glm::lerp(previousVertex.y, p_polygon.Vertices[i].y, t),
				glm::lerp(previousVertex.z, p_polygon.Vertices[i].z, t),
				glm::lerp(previousVertex.w, p_polygon.Vertices[i].w, t)
			};

			insideTexCoords[insideVerticesCount] = {
				glm::lerp(previousTexCoords.x, p_polygon.TexCoords[i].x, t),
				glm::lerp(previousTexCoords.y, p_polygon.TexCoords[i].y, t)
			};

			insideNormals[insideVerticesCount] = {
				glm::lerp(previousNormal.x, p_polygon.Normals[i].x, t),
				glm::lerp(previousNormal.y, p_polygon.Normals[i].y, t),
				glm::lerp(previousNormal.z, p_polygon.Normals[i].z, t)
			};

			for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
			{
				insideVaryings[insideVerticesCount][j] = glm::lerp(previousVarying[j], p_polygon.Varyings[i][j], t);
			}

			insideVerticesCount++;
		}

		if (currentDotValue >= 0.0f)
		{
			insideVertices[insideVerticesCount] = p_polygon.Vertices[i];
			insideTexCoords[insideVerticesCount] = p_polygon.TexCoords[i];
			insideNormals[insideVerticesCount] = p_polygon.Normals[i];

			for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
			{
				insideVaryings[insideVerticesCount][j] = p_polygon.Varyings[i][j];
			}

			insideVerticesCount++;
		}

		previousVertex = p_polygon.Vertices[i];
		previousTexCoords = p_polygon.TexCoords[i];
		previousNormal = p_polygon.Normals[i];
		previousVarying = p_polygon.Varyings[i];
		previousDotValue = currentDotValue;
	}

	p_polygon.VerticesCount = insideVerticesCount;

	for (int i = 0; i < insideVerticesCount; i++)
	{
		p_polygon.Vertices[i] = insideVertices[i];
		p_polygon.TexCoords[i] = insideTexCoords[i];
		p_polygon.Normals[i] = insideNormals[i];

		for (int j = 0; j < p_polygon.VaryingsDataSize; j++)
		{
			p_polygon.Varyings[i][j] = insideVaryings[i][j];
		}
	}
}

void ProcessPoint(const AmberGL::Geometry::Vertex& p_vertex)
{
	glm::vec4 processedVertex = RenderContext.Program->ProcessVertex(p_vertex, 0);

	for (const auto& plane : ClippingFrustum)
	{
		if (!plane.IsInFront(processedVertex))
			return;
	}

	glm::vec3 vertexScreenPosition = ComputeScreenSpaceCoordinate(processedVertex);
	glm::vec2 vertexNormalizedPosition = ComputeNormalizedDeviceCoordinate(vertexScreenPosition);
	glm::vec2 vertexRasterPosition = ComputeRasterSpaceCoordinate(vertexNormalizedPosition);

	glm::vec4 transformedVertex(vertexRasterPosition.x, vertexRasterPosition.y, vertexScreenPosition.z, processedVertex.w);

	AmberGL::Geometry::Point point(transformedVertex);

	RasterizePoint(point);
}

void RasterizePoint(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec4& p_point)
{
	const int width = static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Width : RenderContext.FrameBufferObject->DepthBuffer->Width);
	const int height = static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Height : RenderContext.FrameBufferObject->DepthBuffer->Height);

	const glm::vec3 centerBarycentricCoords = p_triangle.GetBarycentricCoordinates({ p_point.x, p_point.y });

	float depth = p_transformedVertices[0].z * centerBarycentricCoords.x +
		p_transformedVertices[1].z * centerBarycentricCoords.y +
		p_transformedVertices[2].z * centerBarycentricCoords.z;

	depth = depth * 0.5f + 0.5f;
	depth = std::max(0.0f, std::min(1.0f, depth));

	float w = p_transformedVertices[0].w * centerBarycentricCoords.x +
		p_transformedVertices[1].w * centerBarycentricCoords.y +
		p_transformedVertices[2].w * centerBarycentricCoords.z;

	const float scaledPointSize = RenderContext.PointSize * (1.0f / std::abs(w));
	const float halfSize = scaledPointSize * 0.5f;

	const int startX = std::max(0, static_cast<int>(std::floor(p_point.x - halfSize)));
	const int endX = std::min(width - 1, static_cast<int>(std::ceil(p_point.x + halfSize)));
	const int startY = std::max(0, static_cast<int>(std::floor(p_point.y - halfSize)));
	const int endY = std::min(height - 1, static_cast<int>(std::ceil(p_point.y + halfSize)));

	for (int y = startY; y <= endY; y++)
	{
		for (int x = startX; x <= endX; x++)
		{
			if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer != nullptr)
			{
				const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ static_cast<float>(x), static_cast<float>(y) });

				if (!(RenderContext.State & AGL_DEPTH_TEST) || DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(x, y)))
				{
					RenderContext.Program->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

					const glm::vec4 color = RenderContext.Program->ProcessFragment();

					for (uint8_t sampleIndex = 0; sampleIndex < RenderContext.Samples; sampleIndex++)
					{
						MSAABuffer->SetPixelSample(x, y, sampleIndex, PackColor(color), depth);
					}
				}
			}
			else
			{
				if (!(RenderContext.State & AGL_DEPTH_TEST) || DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(x, y)))
				{
					const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ static_cast<float>(x), static_cast<float>(y) });

					RenderContext.Program->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

					const glm::vec4 color = RenderContext.Program->ProcessFragment();

					if (RenderContext.FrameBufferObject->ColorBuffer != nullptr)
					{
						RenderContext.FrameBufferObject->ColorBuffer->SetPixel(x, y, PackColor(glm::mix(UnpackColor(RenderContext.FrameBufferObject->ColorBuffer->GetPixel(x, y)), color, color.w)));
					}

					if (RenderContext.State & AGL_DEPTH_WRITE)
					{
						RenderContext.FrameBufferObject->DepthBuffer->SetPixel(x, y, depth);
					}
				}
			}
		}
	}
}

void RasterizePoint(const AmberGL::Geometry::Point& p_point)
{
	const int width = static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Width : RenderContext.FrameBufferObject->DepthBuffer->Width);
	const int height = static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Height : RenderContext.FrameBufferObject->DepthBuffer->Height);

	float depth = p_point.ScreenSpaceCoordinate.z;

	depth = depth * 0.5f + 0.5f;
	depth = std::max(0.0f, std::min(1.0f, depth));

	const float scaledPointSize = RenderContext.PointSize * (1.0f / std::abs(p_point.ScreenSpaceCoordinate.w));
	const float halfSize = scaledPointSize * 0.5f;

	const int startX = std::max(0, static_cast<int>(std::floor(p_point.ScreenSpaceCoordinate.x - halfSize)));
	const int endX = std::min(width - 1, static_cast<int>(std::ceil(p_point.ScreenSpaceCoordinate.x + halfSize)));
	const int startY = std::max(0, static_cast<int>(std::floor(p_point.ScreenSpaceCoordinate.y - halfSize)));
	const int endY = std::min(height - 1, static_cast<int>(std::ceil(p_point.ScreenSpaceCoordinate.y + halfSize)));

	for (int y = startY; y <= endY; y++)
	{
		for (int x = startX; x <= endX; x++)
		{
			if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer != nullptr)
			{
				if (!(RenderContext.State & AGL_DEPTH_TEST) || DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(x, y)))
				{
					RenderContext.Program->ProcessPointInterpolation(p_point.ScreenSpaceCoordinate.w);

					const glm::vec4 color = RenderContext.Program->ProcessFragment();

					for (uint8_t sampleIndex = 0; sampleIndex < RenderContext.Samples; sampleIndex++)
					{
						MSAABuffer->SetPixelSample(x, y, sampleIndex, PackColor(color), depth);
					}
				}
			}
			else
			{
				if (!(RenderContext.State & AGL_DEPTH_TEST) || DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(x, y)))
				{
					RenderContext.Program->ProcessPointInterpolation(p_point.ScreenSpaceCoordinate.w);

					if (RenderContext.FrameBufferObject->ColorBuffer != nullptr)
					{
						const glm::vec4 color = RenderContext.Program->ProcessFragment();

						RenderContext.FrameBufferObject->ColorBuffer->SetPixel(x, y, PackColor(glm::mix(UnpackColor(RenderContext.FrameBufferObject->ColorBuffer->GetPixel(x, y)), color, color.w)));
					}

					if (RenderContext.State & AGL_DEPTH_WRITE)
					{
						RenderContext.FrameBufferObject->DepthBuffer->SetPixel(x, y, depth);
					}
				}
			}
		}
	}
}

void ProcessLine(const AmberGL::Geometry::Vertex& p_vertex0, const AmberGL::Geometry::Vertex& p_vertex1)
{
	std::array<glm::vec4, 2> processedVertices
	{
		RenderContext.Program->ProcessVertex(p_vertex0, 0),
		RenderContext.Program->ProcessVertex(p_vertex1, 1)
	};
	
	AmberGL::Geometry::Polygon currentPoly;
	currentPoly.Vertices[0] = processedVertices[0];
	currentPoly.Vertices[1] = processedVertices[1];
	currentPoly.TexCoords[0] = p_vertex0.texCoords;
	currentPoly.TexCoords[1] = p_vertex1.texCoords;
	currentPoly.Normals[0] = p_vertex0.normal;
	currentPoly.Normals[1] = p_vertex1.normal;
	currentPoly.VerticesCount = 2;

	auto& programVaryings = RenderContext.Program->GetVaryings();
	int totalSize = 0;

	for (const auto& [name, varying] : programVaryings)
	{
		totalSize += RenderContext.Program->GetTypeCount(varying.Type);
	}

	currentPoly.VaryingsDataSize = totalSize;

	uint16_t offset = 0;
	for (auto& [name, varying] : programVaryings)
	{
		uint8_t typeCount = RenderContext.Program->GetTypeCount(varying.Type);
		for (uint8_t vertIdx = 0; vertIdx < 2; vertIdx++)
		{
			for (uint8_t i = 0; i < typeCount; i++)
			{
				currentPoly.Varyings[vertIdx][offset + i] = varying.Data[vertIdx][i];
			}
		}
		offset += typeCount;
	}

	for (const auto& plane : ClippingFrustum)
	{
		plane.ClipLine(currentPoly);
	}

	if (currentPoly.VerticesCount < 2)
		return;

	offset = 0;
	for (auto& [name, varying] : programVaryings)
	{
		uint8_t typeCount = RenderContext.Program->GetTypeCount(varying.Type);
		for (uint8_t j = 0; j < typeCount; j++)
		{
			varying.Data[0][j] = currentPoly.Varyings[0][offset + j];
			varying.Data[1][j] = currentPoly.Varyings[1][offset + j];
		}
		offset += typeCount;
	}

	std::array<glm::vec4, 2> transformedVertices;

	for (int i = 0; i < 2; i++)
	{
		glm::vec3 screenPos = ComputeScreenSpaceCoordinate(currentPoly.Vertices[i]);
		glm::vec2 ndcPos = ComputeNormalizedDeviceCoordinate(screenPos);
		glm::vec2 rasterPos = ComputeRasterSpaceCoordinate(ndcPos);

		transformedVertices[i] = currentPoly.Vertices[i];
		transformedVertices[i].x = rasterPos.x;
		transformedVertices[i].y = rasterPos.y;
		transformedVertices[i].z = screenPos.z;
	}

	RasterizeLineDirectly(transformedVertices);
}

void RasterizeLineDirectly(const std::array<glm::vec4, 2>& p_transformedVertices)
{
	float startX = p_transformedVertices[0].x;
	float startY = p_transformedVertices[0].y;
	float startZ = p_transformedVertices[0].z;
	float startW = p_transformedVertices[0].w;
	float endX = p_transformedVertices[1].x;
	float endY = p_transformedVertices[1].y;
	float endZ = p_transformedVertices[1].z;
	float endW = p_transformedVertices[1].w;

	int width = RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Width : RenderContext.FrameBufferObject->DepthBuffer->Width;
	int height = RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Height : RenderContext.FrameBufferObject->DepthBuffer->Height;

	float deltaX = endX - startX;
	float deltaY = endY - startY;
	float lineLength = std::sqrt(deltaX * deltaX + deltaY * deltaY);

	float normalizedDirX = deltaX / lineLength;
	float normalizedDirY = deltaY / lineLength;

	float perpendicularsX = -normalizedDirY;
	float perpendicularsY = normalizedDirX;

	float safeStartW = std::abs(startW);
	float safeEndW = std::abs(endW);

	float invDotProduct = 1.0f / (deltaX * deltaX + deltaY * deltaY);

	if (RenderContext.LineWidth == 1.0f) 
	{
		int startPointX = static_cast<int>(std::floor(startX + 0.5f));
		int startPointY = static_cast<int>(std::floor(startY + 0.5f));
		int endPointX = static_cast<int>(std::floor(endX + 0.5f));
		int endPointY = static_cast<int>(std::floor(endY + 0.5f));

		if ((startPointX < 0 && endPointX  < 0) || (startPointX >= width && endPointX  >= width) 
			|| (startPointY  < 0 && endPointY  < 0) || (startPointY  >= height && endPointY  >= height)) 
		{
			return;
		}

		bool isSteep = std::abs(endPointY  - startPointY) > std::abs(endPointX  - startPointX);

		if (isSteep) 
		{
			std::swap(startPointX, startPointY);
			std::swap(endPointX , endPointY);
		}

		if (startPointX > endPointX)
		{
			std::swap(startPointX, endPointX);
			std::swap(startPointY , endPointY);
			std::swap(startX, endX);
			std::swap(startY, endY);
			std::swap(startZ, endZ);
			std::swap(startW, endW);
		}

		int xDiff = endPointX  - startPointX;
		int yDiff = std::abs(endPointY  - startPointY);
		int bresenhamError = xDiff * 0.5f;

		int yDirection = (startPointY  < endPointY) ? 1 : -1;
		int currentY = startPointY;

		for (int currentX = startPointX; currentX <= endPointX ; ++currentX)
		{
			int pixelX = isSteep ? currentY : currentX;
			int pixelY = isSteep ? currentX : currentY;

			if (pixelX >= 0 && pixelX < width && pixelY >= 0 && pixelY < height) 
			{
				//TODO: Implement Xiaolin Wu's algorithm. The MSAA makes no sense for Bresenham line here.
				//MSAA not effective of course because Bresenham produces binary (fully covered/not covered) pixels.
				if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer)
				{
					uint8_t gridSize = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));
					uint8_t sampleCount = 0;

					for (uint8_t j = 0; j < gridSize; j++)
					{
						for (uint8_t i = 0; i < gridSize; i++)
						{
							if (sampleCount >= RenderContext.Samples)
								break;

							float samplePosX = pixelX + (i + 0.5f) / gridSize;
							float samplePosY = pixelY + (j + 0.5f) / gridSize;

							float interpolationParam = ((samplePosX - startX) * deltaX + (samplePosY - startY) * deltaY) * invDotProduct;
							interpolationParam = std::max(0.0f, std::min(1.0f, interpolationParam));

							float invWInterpolated = (1.0f - interpolationParam) / safeStartW + interpolationParam / safeEndW;
							float interpolatedZ = (1.0f - interpolationParam) * startZ / safeStartW + interpolationParam * endZ / safeEndW;
							float sampleDepth = (interpolatedZ / invWInterpolated) * 0.5f + 0.5f;

							RenderContext.Program->ProcessInterpolation(
								{ 1.0f - interpolationParam, interpolationParam, 0.0f },
								p_transformedVertices[0].w,
								p_transformedVertices[1].w,
								1.0f);

							const glm::vec4 color = RenderContext.Program->ProcessFragment();
							MSAABuffer->SetPixelSample(pixelX, pixelY, sampleCount, PackColor(color), sampleDepth);

							sampleCount++;
						}
					}
				}
				else
				{
					float pixelPosX = isSteep ? pixelY + 0.5f : pixelX + 0.5f;
					float pixelPosY = isSteep ? pixelX + 0.5f : pixelY + 0.5f;

					float interpolationParam = ((pixelPosX - startX) * deltaX + (pixelPosY - startY) * deltaY) * invDotProduct;
					interpolationParam = std::max(0.0f, std::min(1.0f, interpolationParam));

					float invWInterpolated = (1.0f - interpolationParam) / safeStartW + interpolationParam / safeEndW;
					float interpolatedZ = (1.0f - interpolationParam) * startZ / safeStartW + interpolationParam * endZ / safeEndW;
					float pixelDepth = (interpolatedZ / invWInterpolated) * 0.5f + 0.5f;

					if (!(RenderContext.State & AGL_DEPTH_TEST) || DepthTest(pixelDepth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(pixelX, pixelY)))
					{
						RenderContext.Program->ProcessInterpolation(
							{ 1.0f - interpolationParam, interpolationParam, 0.0f },
							p_transformedVertices[0].w,
							p_transformedVertices[1].w,
							1.0f);

						if (RenderContext.FrameBufferObject->ColorBuffer)
						{
							const glm::vec4 color = RenderContext.Program->ProcessFragment();
							RenderContext.FrameBufferObject->ColorBuffer->SetPixel(pixelX, pixelY, PackColor(glm::mix(UnpackColor(RenderContext.FrameBufferObject->ColorBuffer->GetPixel(pixelX, pixelY)), color, color.w)));
						}

						if (RenderContext.State & AGL_DEPTH_WRITE)
						{
							RenderContext.FrameBufferObject->DepthBuffer->SetPixel(pixelX, pixelY, pixelDepth);
						}
					}
				}
			}

			bresenhamError -= yDiff;

			if (bresenhamError < 0) 
			{
				currentY += yDirection;
				bresenhamError += xDiff;
			}
		}
	}
	else
	{
		float halfWidthStart = (RenderContext.LineWidth * 0.5f) / safeStartW;
		float halfWidthEnd = (RenderContext.LineWidth * 0.5f) / safeEndW;

		float corner0X = startX - perpendicularsX * halfWidthStart;
		float corner0Y = startY - perpendicularsY * halfWidthStart;

		float corner1X = startX + perpendicularsX * halfWidthStart;
		float corner1Y = startY + perpendicularsY * halfWidthStart;

		float corner2X = endX + perpendicularsX * halfWidthEnd;
		float corner2Y = endY + perpendicularsY * halfWidthEnd;

		float corner3X = endX - perpendicularsX * halfWidthEnd;
		float corner3Y = endY - perpendicularsY * halfWidthEnd;

		float edge0X = corner1X - corner0X;  float edge0Y = corner1Y - corner0Y;
		float edge1X = corner2X - corner1X;  float edge1Y = corner2Y - corner1Y;
		float edge2X = corner3X - corner2X;  float edge2Y = corner3Y - corner2Y;
		float edge3X = corner0X - corner3X;  float edge3Y = corner0Y - corner3Y;

		float minBoxX = std::min(std::min(corner0X, corner1X), std::min(corner2X, corner3X));
		float maxBoxX = std::max(std::max(corner0X, corner1X), std::max(corner2X, corner3X));
		float minBoxY = std::min(std::min(corner0Y, corner1Y), std::min(corner2Y, corner3Y));
		float maxBoxY = std::max(std::max(corner0Y, corner1Y), std::max(corner2Y, corner3Y));

		int boxStartX = std::max(0, static_cast<int>(minBoxX));
		int boxEndX = std::min(width - 1, static_cast<int>(maxBoxX));
		int boxStartY = std::max(0, static_cast<int>(minBoxY));
		int boxEndY = std::min(height - 1, static_cast<int>(maxBoxY));

		for (int y = boxStartY; y <= boxEndY; y++)
		{
			for (int x = boxStartX; x <= boxEndX; x++)
			{
				if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer)
				{
					uint8_t gridSize = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));
					uint8_t sampleCount = 0;

					for (uint8_t j = 0; j < gridSize; j++)
					{
						for (uint8_t i = 0; i < gridSize; i++)
						{
							if (sampleCount >= RenderContext.Samples)
								break;

							float samplePosX = x + (i + 0.5f) / gridSize;
							float samplePosY = y + (j + 0.5f) / gridSize;

							float sampleCross0 = (samplePosX - corner0X) * edge0Y - (samplePosY - corner0Y) * edge0X;
							float sampleCross1 = (samplePosX - corner1X) * edge1Y - (samplePosY - corner1Y) * edge1X;
							float sampleCross2 = (samplePosX - corner2X) * edge2Y - (samplePosY - corner2Y) * edge2X;
							float sampleCross3 = (samplePosX - corner3X) * edge3Y - (samplePosY - corner3Y) * edge3X;

							if (!((sampleCross0 >= 0 && sampleCross1 >= 0 && sampleCross2 >= 0 && sampleCross3 >= 0)
								|| (sampleCross0 <= 0 && sampleCross1 <= 0 && sampleCross2 <= 0 && sampleCross3 <= 0)))
							{
								sampleCount++;
								continue;
							}

							float sampleInterpolationParam = ((samplePosX - startX) * deltaX + (samplePosY - startY) * deltaY) * invDotProduct;
							sampleInterpolationParam = std::max(0.0f, std::min(1.0f, sampleInterpolationParam));

							float sampleInvW = (1.0f - sampleInterpolationParam) / safeStartW + sampleInterpolationParam / safeEndW;
							float sampleZ = (1.0f - sampleInterpolationParam) * startZ / safeStartW + sampleInterpolationParam * endZ / safeEndW;
							float sampleDepth = (sampleZ / sampleInvW) * 0.5f + 0.5f;

							RenderContext.Program->ProcessInterpolation(
								{ 1.0f - sampleInterpolationParam, sampleInterpolationParam, 0.0f },
								p_transformedVertices[0].w,
								p_transformedVertices[1].w,
								1.0f);

							const glm::vec4 color = RenderContext.Program->ProcessFragment();

							MSAABuffer->SetPixelSample(x, y, sampleCount, PackColor(color), sampleDepth);

							sampleCount++;
						}
					}
				}
				else
				{
					float pixelCenterX = x + 0.5f;
					float pixelCenterY = y + 0.5f;

					float pixelCross0 = (pixelCenterX - corner0X) * edge0Y - (pixelCenterY - corner0Y) * edge0X;
					float pixelCross1 = (pixelCenterX - corner1X) * edge1Y - (pixelCenterY - corner1Y) * edge1X;
					float pixelCross2 = (pixelCenterX - corner2X) * edge2Y - (pixelCenterY - corner2Y) * edge2X;
					float pixelCross3 = (pixelCenterX - corner3X) * edge3Y - (pixelCenterY - corner3Y) * edge3X;

					if (!((pixelCross0 >= 0 && pixelCross1 >= 0 && pixelCross2 >= 0 && pixelCross3 >= 0)
						|| (pixelCross0 <= 0 && pixelCross1 <= 0 && pixelCross2 <= 0 && pixelCross3 <= 0)))
						continue;

					float pixelInterpolationParam = ((pixelCenterX - startX) * deltaX + (pixelCenterY - startY) * deltaY) * invDotProduct;
					pixelInterpolationParam = std::max(0.0f, std::min(1.0f, pixelInterpolationParam));

					float pixelInvW = (1.0f - pixelInterpolationParam) / safeStartW + pixelInterpolationParam / safeEndW;
					float pixelZ = (1.0f - pixelInterpolationParam) * startZ / safeStartW + pixelInterpolationParam * endZ / safeEndW;
					float pixelDepth = (pixelZ / pixelInvW) * 0.5f + 0.5f;

					if (RenderContext.State & AGL_DEPTH_TEST && DepthTest(pixelDepth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(x, y)))
					{
						RenderContext.Program->ProcessInterpolation(
							{ 1.0f - pixelInterpolationParam, pixelInterpolationParam, 0.0f },
							p_transformedVertices[0].w,
							p_transformedVertices[1].w,
							1.0f);

						if (RenderContext.FrameBufferObject->ColorBuffer)
						{
							const glm::vec4 color = RenderContext.Program->ProcessFragment();

							RenderContext.FrameBufferObject->ColorBuffer->SetPixel(x, y, PackColor(glm::mix(UnpackColor(RenderContext.FrameBufferObject->ColorBuffer->GetPixel(x, y)), color, color.w)));
						}

						if (RenderContext.State & AGL_DEPTH_WRITE)
						{
							RenderContext.FrameBufferObject->DepthBuffer->SetPixel(x, y, pixelDepth);
						}
					}
				}
			}
		}
	}

	//ROUNDED LINES TEST, WORKING WELL EXCEPT WHEN LINE IS ALIGNED WITH THE CAMERA, LINE WIDTH BECOME WRONG DUE TO PERSPECTIVE ISSUE
	/*float x0 = p_transformedVertices[0].x;
	float y0 = p_transformedVertices[0].y;
	float z0 = p_transformedVertices[0].z;
	float x1 = p_transformedVertices[1].x;
	float y1 = p_transformedVertices[1].y;
	float z1 = p_transformedVertices[1].z;
	float w0 = p_transformedVertices[0].w;
	float w1 = p_transformedVertices[1].w;

	float dx = x1 - x0;
	float dy = y1 - y0;

	const int width = static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Width : RenderContext.FrameBufferObject->DepthBuffer->Width);
	const int height = static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Height : RenderContext.FrameBufferObject->DepthBuffer->Height);

	float thickness0 = std::min(RenderContext.LineWidth * (1.0f / w0), RenderContext.LineWidth * 2.0f);
	float thickness1 = std::min(RenderContext.LineWidth * (1.0f / w1), RenderContext.LineWidth * 2.0f);

	thickness0 = std::max(thickness0, 1.0f);
	thickness1 = std::max(thickness1, 1.0f);

	float maxThickness = std::max(thickness0, thickness1);
	float halfMaxThickness = maxThickness * 0.5f;

	float padding = 1.0f;
	float minX = std::min(x0, x1) - halfMaxThickness - padding;
	float maxX = std::max(x0, x1) + halfMaxThickness + padding;
	float minY = std::min(y0, y1) - halfMaxThickness - padding;
	float maxY = std::max(y0, y1) + halfMaxThickness + padding;

	int startX = std::max(0, static_cast<int>(std::floor(minX)));
	int endX = std::min(width - 1, static_cast<int>(std::ceil(maxX)));
	int startY = std::max(0, static_cast<int>(std::floor(minY)));
	int endY = std::min(height - 1, static_cast<int>(std::ceil(maxY)));

	float invLen2 = 1.0f / (dx * dx + dy * dy);

	for (int y = startY; y <= endY; y++)
	{
		for (int x = startX; x <= endX; x++)
		{
			if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer)
			{
				uint8_t gridSize = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));

				uint8_t sampleCount = 0;

				for (uint8_t j = 0; j < gridSize; j++)
				{
					for (uint8_t i = 0; i < gridSize; i++)
					{
						if (sampleCount >= RenderContext.Samples)
							break;

						const float sampleX = x + (i + 0.5f) / gridSize;
						const float sampleY = y + (j + 0.5f) / gridSize;

						const float t = ((sampleX - x0) * dx + (sampleY - y0) * dy) * invLen2;
						const float clampedT = std::max(0.0f, std::min(1.0f, t));

						const float closestX = x0 + clampedT * dx;
						const float closestY = y0 + clampedT * dy;

						float invW = (1.0f - clampedT) / w0 + clampedT / w1;
						float correctW = 1.0f / invW;

						float z = (1.0f - clampedT) * z0 / w0 + clampedT * z1 / w1;
						float correctZ = z * correctW;

						float depth = correctZ * 0.5f + 0.5f;
						depth = std::max(0.0f, std::min(1.0f, depth));

						float interpolatedThickness = thickness0 * (1.0f - clampedT) + thickness1 * clampedT;
						float halfThickness = interpolatedThickness * 0.5f;

						const float distance = std::sqrt(
							(sampleX - closestX) * (sampleX - closestX) +
							(sampleY - closestY) * (sampleY - closestY));

						if (distance <= halfThickness)
						{
							const glm::vec3 weightedCoords(1.0f - clampedT, clampedT, 0.0f);

							RenderContext.Program->ProcessInterpolation(weightedCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, 1.0f);

							const glm::vec4 color = RenderContext.Program->ProcessFragment();

							MSAABuffer->SetPixelSample(x, y, sampleCount, PackColor(color), depth);
						}

						sampleCount++;
					}
				}
			}
			else
			{
				const float pixelX = x + 0.5f;
				const float pixelY = y + 0.5f;

				const float t = ((pixelX - x0) * dx + (pixelY - y0) * dy) * invLen2;
				const float clampedT = std::max(0.0f, std::min(1.0f, t));

				const float closestX = x0 + clampedT * dx;
				const float closestY = y0 + clampedT * dy;

				float invW = (1.0f - clampedT) / w0 + clampedT / w1;
				float correctW = 1.0f / invW;

				float z = (1.0f - clampedT) * z0 / w0 + clampedT * z1 / w1;
				float correctZ = z * correctW;

				float interpolatedThickness = thickness0 * (1.0f - clampedT) + thickness1 * clampedT;
				float halfThickness = interpolatedThickness * 0.5f;

				const float distance = std::sqrt(
					(pixelX - closestX) * (pixelX - closestX) +
					(pixelY - closestY) * (pixelY - closestY));

				float depth = correctZ * 0.5f + 0.5f;
				depth = std::max(0.0f, std::min(1.0f, depth));

				if (distance <= halfThickness)
				{
					if (!(RenderContext.State & AGL_DEPTH_TEST) || PerformDepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(x, y)))
					{
						const glm::vec3 weightedCoords(1.0f - clampedT, clampedT, 0.0f);

						RenderContext.Program->ProcessInterpolation(weightedCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, 1.0f);

						if (RenderContext.FrameBufferObject->ColorBuffer != nullptr)
						{
							const glm::vec4 color = RenderContext.Program->ProcessFragment();

							RenderContext.FrameBufferObject->ColorBuffer->SetPixel(x, y, PackColor(glm::mix(UnpackColor(RenderContext.FrameBufferObject->ColorBuffer->GetPixel(x, y)), color, color.w)));
						}

						if (RenderContext.State & AGL_DEPTH_WRITE)
						{
							RenderContext.FrameBufferObject->DepthBuffer->SetPixel(x, y, depth);
						}
					}
				}
			}
		}
	}*/
}

void RasterizeLine(const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_start, const glm::vec4& p_end)
{
	float startX = p_start.x;
	float startY = p_start.y;
	float startZ = p_start.z;
	float startW = p_start.w;
	float endX = p_end.x;
	float endY = p_end.y;
	float endZ = p_end.z;
	float endW = p_end.w;

	float deltaX = endX - startX;
	float deltaY = endY - startY;
	float lineLength = std::sqrt(deltaX * deltaX + deltaY * deltaY);

	float safeStartW = std::abs(startW);
	float safeEndW = std::abs(endW);

	const int frameBufferWidth = static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Width : RenderContext.FrameBufferObject->DepthBuffer->Width);
	const int frameBufferHeight = static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Height : RenderContext.FrameBufferObject->DepthBuffer->Height);

	float thicknessStart = std::min(RenderContext.LineWidth * (1.0f / safeStartW), RenderContext.LineWidth);
	float thicknessEnd = std::min(RenderContext.LineWidth * (1.0f / safeEndW), RenderContext.LineWidth);

	thicknessStart = std::max(thicknessStart, 1.0f);
	thicknessEnd = std::max(thicknessEnd, 1.0f);

	float maxThickness = std::max(thicknessStart, thicknessEnd);
	float halfMaxThickness = maxThickness * 0.5f;

	float padding = 1.0f;
	float minX = std::min(startX, endX) - halfMaxThickness - padding;
	float maxX = std::max(startX, endX) + halfMaxThickness + padding;
	float minY = std::min(startY, endY) - halfMaxThickness - padding;
	float maxY = std::max(startY, endY) + halfMaxThickness + padding;

	int startPointX = std::max(0, static_cast<int>(std::floor(minX)));
	int endPointX = std::min(frameBufferWidth - 1, static_cast<int>(std::ceil(maxX)));
	int startPointY = std::max(0, static_cast<int>(std::floor(minY)));
	int endPointY = std::min(frameBufferHeight - 1, static_cast<int>(std::ceil(maxY)));

	float invLengthSquared = 1.0f / (deltaX * deltaX + deltaY * deltaY);

	for (int scanY = startPointY; scanY <= endPointY; scanY++)
	{
		for (int scanX = startPointX; scanX <= endPointX; scanX++)
		{
			if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer)
			{
				uint8_t gridSideLength = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));
				uint8_t currentSampleIndex = 0;

				for (uint8_t gridRow = 0; gridRow < gridSideLength; gridRow++)
				{
					for (uint8_t gridCol = 0; gridCol < gridSideLength; gridCol++)
					{
						if (currentSampleIndex >= RenderContext.Samples)
							break;

						const float samplePosX = scanX + (gridCol + 0.5f) / gridSideLength;
						const float samplePosY = scanY + (gridRow + 0.5f) / gridSideLength;

						const float t = ((samplePosX - startX) * deltaX + (samplePosY - startY) * deltaY) * invLengthSquared;
						const float clampedT = std::max(0.0f, std::min(1.0f, t));

						const float closestPosX = startX + clampedT * deltaX;
						const float closestPosY = startY + clampedT * deltaY;

						float invWInterpolated = (1.0f - clampedT) / safeStartW + clampedT / safeEndW;
						float interpolatedW = 1.0f / invWInterpolated;

						float interpolatedZ = (1.0f - clampedT) * startZ / safeStartW + clampedT * endZ / safeEndW;
						float finalZ = interpolatedZ * interpolatedW;

						float sampleDepth = finalZ * 0.5f + 0.5f;
						sampleDepth = std::max(0.0f, std::min(1.0f, sampleDepth));

						float interpolatedThickness = thicknessStart * (1.0f - clampedT) + thicknessEnd * clampedT;
						float halfInterpolatedThickness = interpolatedThickness * 0.5f;

						const float distanceFromLine = (samplePosX - closestPosX) * (samplePosX - closestPosX) + (samplePosY - closestPosY) * (samplePosY - closestPosY);

						if (distanceFromLine <= halfInterpolatedThickness * halfInterpolatedThickness)
						{
							const glm::vec3 barycentricWeights(1.0f - clampedT, clampedT, 0.0f);

							RenderContext.Program->ProcessInterpolation(
								barycentricWeights,
								transformedVertices[0].w,
								transformedVertices[1].w,
								transformedVertices[2].w);

							const glm::vec4 color = RenderContext.Program->ProcessFragment();

							MSAABuffer->SetPixelSample(scanX, scanY, currentSampleIndex, PackColor(color), sampleDepth);
						}

						currentSampleIndex++;
					}
				}
			}
			else
			{
				const float pixelCenterX = scanX + 0.5f;
				const float pixelCenterY = scanY + 0.5f;

				const float t = ((pixelCenterX - startX) * deltaX + (pixelCenterY - startY) * deltaY) * invLengthSquared;
				const float clampedT = std::max(0.0f, std::min(1.0f, t));

				const float closestPosX = startX + clampedT * deltaX;
				const float closestPosY = startY + clampedT * deltaY;

				float invWInterpolated = (1.0f - clampedT) / safeStartW + clampedT / safeEndW;
				float interpolatedW = 1.0f / invWInterpolated;

				float interpolatedZ = (1.0f - clampedT) * startZ / safeStartW + clampedT * endZ / safeEndW;
				float finalZ = interpolatedZ * interpolatedW;

				float interpolatedThickness = thicknessStart * (1.0f - clampedT) + thicknessEnd * clampedT;
				float halfInterpolatedThickness = interpolatedThickness * 0.5f;

				const float distanceFromLine = (pixelCenterX - closestPosX) * (pixelCenterX - closestPosX) + (pixelCenterY - closestPosY) * (pixelCenterY - closestPosY);

				float pixelDepth = finalZ * 0.5f + 0.5f;
				pixelDepth = std::max(0.0f, std::min(1.0f, pixelDepth));

				if (distanceFromLine <= halfInterpolatedThickness * halfInterpolatedThickness)
				{
					if (!(RenderContext.State & AGL_DEPTH_TEST) || DepthTest(pixelDepth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(scanX, scanY)))
					{
						const glm::vec3 barycentricWeights(1.0f - clampedT, clampedT, 0.0f);

						RenderContext.Program->ProcessInterpolation(
							barycentricWeights,
							transformedVertices[0].w,
							transformedVertices[1].w,
							transformedVertices[2].w);
						const glm::vec4 color = RenderContext.Program->ProcessFragment();

						if (RenderContext.FrameBufferObject->ColorBuffer != nullptr)
						{
							RenderContext.FrameBufferObject->ColorBuffer->SetPixel(scanX, scanY, PackColor(glm::mix(UnpackColor(RenderContext.FrameBufferObject->ColorBuffer->GetPixel(scanX, scanY)), color, color.w)));
						}

						if (RenderContext.State & AGL_DEPTH_WRITE)
						{
							RenderContext.FrameBufferObject->DepthBuffer->SetPixel(scanX, scanY, pixelDepth);
						}
					}
				}
			}
		}
	}
}

void RasterizeTriangleWireframe(const std::array<glm::vec4, 3>& transformedVertices)
{
	RasterizeLine(transformedVertices, transformedVertices[0], transformedVertices[1]);
	RasterizeLine(transformedVertices, transformedVertices[1], transformedVertices[2]);
	RasterizeLine(transformedVertices, transformedVertices[2], transformedVertices[0]);
}

void RasterizeTrianglePoints(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices)
{
	RasterizePoint(p_triangle, transformedVertices, transformedVertices[0]);
	RasterizePoint(p_triangle, transformedVertices, transformedVertices[1]);
	RasterizePoint(p_triangle, transformedVertices, transformedVertices[2]);
}

void ApplyMSAA()
{
	if (!RenderContext.FrameBufferObject->ColorBuffer)
		return;

	const uint32_t width = RenderContext.FrameBufferObject->ColorBuffer->Width;
	const uint32_t height = RenderContext.FrameBufferObject->ColorBuffer->Height;

	float depth = 0.0f;

	for (uint32_t x = 0; x < width; x++)
	{
		for (uint32_t y = 0; y < height; y++)
		{
			glm::ivec4 color = { 0, 0, 0, 0 };
			depth = 0.0f;

			for (uint8_t sampleIndex = 0; sampleIndex < RenderContext.Samples; ++sampleIndex)
			{
				const auto& sample = MSAABuffer->GetSample(x, y, sampleIndex);
				color.x += static_cast<uint8_t>(sample.Color >> 24);
				color.y += static_cast<uint8_t>(sample.Color >> 16);
				color.z += static_cast<uint8_t>(sample.Color >> 8);
				color.w += static_cast<uint8_t>(sample.Color);
				depth += sample.Depth;
			}

			color.x /= RenderContext.Samples;
			color.y /= RenderContext.Samples;
			color.z /= RenderContext.Samples;
			color.w /= RenderContext.Samples;
			depth /= static_cast<float>(RenderContext.Samples);

			glm::vec4 currentPixelColor = UnpackColor(RenderContext.FrameBufferObject->ColorBuffer->GetPixel(x, y));

			float alpha = static_cast<float>(color.w) / 255.0f;

			glm::vec4 sampledColor(
				color.x / 255.0f,
				color.y / 255.0f,
				color.z / 255.0f,
				color.w / 255.0f
			);

			RenderContext.FrameBufferObject->ColorBuffer->SetPixel(x, y, PackColor(glm::mix(currentPixelColor, sampledColor, alpha)));

			if (RenderContext.State & AGL_DEPTH_WRITE)
			{
				RenderContext.FrameBufferObject->DepthBuffer->SetPixel(x, y, depth);
			}
		}
	}
}

bool DepthTest(float p_fragmentDepth, float p_bufferDepth)
{
	switch (RenderContext.DepthFunc)
	{
	case AGL_NEVER:
		return false;
	case AGL_LESS:
		return p_fragmentDepth < p_bufferDepth;
	case AGL_EQUAL:
		return p_fragmentDepth == p_bufferDepth;
	case AGL_LEQUAL:
		return p_fragmentDepth <= p_bufferDepth;
	case AGL_GREATER:
		return p_fragmentDepth > p_bufferDepth;
	case AGL_NOTEQUAL:
		return p_fragmentDepth != p_bufferDepth;
	case AGL_GEQUAL:
		return p_fragmentDepth >= p_bufferDepth;
	case AGL_ALWAYS:
		return true;
	default:
		return p_fragmentDepth < p_bufferDepth;
	}
}

void AmberGL::GenVertexArrays(uint32_t p_count, uint32_t* p_arrays)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		AmberGL::SoftwareRenderer::RenderObject::VertexArrayObject vertexArrayObject;
		vertexArrayObject.ID = VertexArrayID;
		vertexArrayObject.BoundArrayBuffer = 0;
		vertexArrayObject.BoundElementBuffer = 0;
		VertexArrayObjects[VertexArrayID] = vertexArrayObject;
		p_arrays[i] = VertexArrayID;
		VertexArrayID++;
	}
}

void AmberGL::DeleteVertexArrays(uint32_t p_count, const uint32_t* p_arrays)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		uint32_t id = p_arrays[i];

		VertexArrayObjects.erase(id);

		if (CurrentVertexArrayObject == id)
			CurrentVertexArrayObject = 0;
	}
}

void AmberGL::BindVertexArray(uint32_t p_array)
{
	if (p_array != 0 && VertexArrayObjects.find(p_array) == VertexArrayObjects.end()) 
	{
		std::cout << "VertexArray with ID " << p_array << " not found!\n";
		return;
	}

	CurrentVertexArrayObject = p_array;
}

void AmberGL::GenBuffers(uint32_t p_count, uint32_t* p_buffers)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		AmberGL::SoftwareRenderer::RenderObject::BufferObject bufferObject;
		bufferObject.ID = BufferID;
		bufferObject.Target = 0;
		bufferObject.Size = 0;
		bufferObject.Data.clear();
		BufferObjects[BufferID] = bufferObject;
		p_buffers[i] = BufferID;

		BufferID++;
	}
}

void AmberGL::DeleteBuffers(uint32_t p_count, const uint32_t* p_buffers)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		uint32_t id = p_buffers[i];

		BufferObjects.erase(id);

		if (CurrentArrayBuffer == id)
			CurrentArrayBuffer = 0;

		if (CurrentElementBuffer == id)
			CurrentElementBuffer = 0;
	}
}

void AmberGL::BindBuffer(uint32_t p_target, uint32_t p_buffer)
{
	if (p_target == AGL_ARRAY_BUFFER) 
	{
		CurrentArrayBuffer = p_buffer;

		if (CurrentVertexArrayObject != 0)
			VertexArrayObjects[CurrentVertexArrayObject].BoundArrayBuffer = p_buffer;
	}
	else if (p_target == AGL_ELEMENT_ARRAY_BUFFER)
	{
		CurrentElementBuffer = p_buffer;

		if (CurrentVertexArrayObject != 0)
			VertexArrayObjects[CurrentVertexArrayObject].BoundElementBuffer = p_buffer;
	}
}

void AmberGL::BufferData(uint32_t p_target, size_t p_size, const void* p_data)
{
	uint32_t currentBuffer = (p_target == AGL_ARRAY_BUFFER) ? CurrentArrayBuffer : (p_target == AGL_ELEMENT_ARRAY_BUFFER) ? CurrentElementBuffer : 0;

	if (currentBuffer == 0) 
	{
		std::cout << "No buffer bound for target " << p_target << "\n";
		return;
	}

	AmberGL::SoftwareRenderer::RenderObject::BufferObject& bufferObject = BufferObjects[currentBuffer];
	bufferObject.Target = p_target;
	bufferObject.Size = p_size;
	bufferObject.Data.resize(p_size);
	std::memcpy(bufferObject.Data.data(), p_data, p_size);
}

void AmberGL::GenTextures(uint32_t p_count, uint32_t* p_textures)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		AmberGL::SoftwareRenderer::RenderObject::TextureObject* texture = new AmberGL::SoftwareRenderer::RenderObject::TextureObject();
		texture->ID = TextureID;
		p_textures[i] = TextureID;
		TextureObjects[TextureID] = texture;
		TextureID++;
	}
}

void AmberGL::DeleteTextures(uint32_t p_count, const uint32_t* p_textures)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		uint32_t id = p_textures[i];

		auto it = TextureObjects.find(id);
		if (it != TextureObjects.end()) 
		{
			delete it->second;
			TextureObjects.erase(it);
		}

		if (CurrentTexture == id)
			CurrentTexture = 0;
	}
}

void AmberGL::BindTexture(uint32_t p_target, uint32_t p_texture)
{
	if (p_target != AGL_TEXTURE_2D) 
	{
		std::cout << "BindTexture: Only AGL_TEXTURE_2D supported.\n";
		return;
	}
	if (p_texture != 0 && TextureObjects.find(p_texture) == TextureObjects.end()) 
	{
		std::cout << "BindTexture: Texture with ID " << p_texture << " not found.\n";
		return;
	}
	CurrentTexture = p_texture;

	if (p_texture == 0)
		BoundTextureUnits[CurrentActiveTextureUnit] = nullptr;
	else
		BoundTextureUnits[CurrentActiveTextureUnit] = TextureObjects[CurrentTexture];
}

void AmberGL::TexImage2D(uint32_t p_target, uint32_t p_level, uint32_t p_internalFormat, uint32_t p_width, uint32_t p_height, uint32_t p_border, uint32_t p_format, uint32_t p_type, const void* p_data)
{
	if (p_target != AGL_TEXTURE_2D) 
	{
		std::cout << "TexImage2D: Only AGL_TEXTURE_2D supported.\n";
		return;
	}

	if (CurrentTexture == 0) 
	{
		std::cout << "TexImage2D: No texture bound.\n";
		return;
	}

	AmberGL::SoftwareRenderer::RenderObject::TextureObject& texture = *TextureObjects[CurrentTexture];
	texture.InternalFormat = p_internalFormat;
	texture.Width = p_width;
	texture.Height = p_height;
	texture.Target = p_target;

	if (p_internalFormat == AGL_RGBA8 && p_type == AGL_UNSIGNED_BYTE)
	{
		if (texture.Data8)
		{
			delete[] texture.Data8;
			texture.Data8 = nullptr;
		}

		texture.Data8 = new uint8_t[p_width * p_height * 4];

		if (p_data) 
		{
			std::memcpy(texture.Data8, p_data, p_width * p_height * 4);
		}
		else
		{
			std::fill(texture.Data8, texture.Data8 + p_width * p_height * 4, 0);
		}
	}
	else if (p_internalFormat == AGL_DEPTH_COMPONENT && p_type == AGL_FLOAT) 
	{
		//TODO: Maybe use 32 bit buffer and 8 bit. splitting depth value.
		if (texture.Data8)
		{
			delete[] texture.Data8;
			texture.Data8 = nullptr;
		}

		texture.Data8 = new uint8_t[p_width * p_height * 4];

		if (p_data) 
		{
			std::memcpy(texture.Data8, p_data, p_width * p_height * 4);
		}
		else 
		{
			std::fill(texture.Data8, texture.Data8 + p_width * p_height * 4, 0);
		}

		/*// Also allocate a Data8 buffer for display (we store depth in R channel)
		if (texture.Data8)
		{
			delete[] texture.Data8; texture.Data8 = nullptr;
		}

		texture.Data8 = new uint8_t[p_width * p_height * 4];*/
	}
	else 
	{
		std::cout << "TexImage2D: Unsupported internalFormat/type combination.\n";
	}
}

void AmberGL::TexParameteri(uint32_t p_target, uint16_t p_pname, uint16_t p_param)
{
	if (p_target != AGL_TEXTURE_2D) 
	{
		std::cout << "TexParameteri: Only AGL_TEXTURE_2D supported.\n";
		return;
	}

	if (CurrentTexture == 0) 
	{
		std::cout << "TexParameteri: No texture bound.\n";
		return;
	}

	AmberGL::SoftwareRenderer::RenderObject::TextureObject& texture = *TextureObjects[CurrentTexture];

	switch (p_pname)
	{
	case AGL_TEXTURE_MIN_FILTER:
		texture.MinFilter = p_param;
		break;
	case AGL_TEXTURE_MAG_FILTER:
		texture.MagFilter = p_param;
		break;
	case AGL_TEXTURE_WRAP_S:
		texture.WrapS = p_param;
		break;
	case AGL_TEXTURE_WRAP_T:
		texture.WrapT = p_param;
		break;
	default:
		std::cout << "TexParameteri: Unsupported pname " << p_param << "\n";
		break;
	}
}

void AmberGL::ActiveTexture(uint32_t p_unit)
{
	if (p_unit >= MAX_TEXTURE_UNITS)
	{
		std::cout << "ActiveTexture: Texture unit " << p_unit << " is out of range (max " << MAX_TEXTURE_UNITS - 1 << ").\n";
		return;
	}

	CurrentActiveTextureUnit = p_unit;
}

void AmberGL::GenerateMipmap(uint32_t p_target)
{
	if (p_target != AGL_TEXTURE_2D)
	{
		std::cout << "BindTexture: Only AGL_TEXTURE_2D supported.\n";
		return;
	}

	AmberGL::SoftwareRenderer::RenderObject::TextureObject& textureObject = *TextureObjects[CurrentTexture];
	AmberGL::SoftwareRenderer::TextureSampler::GenerateMipmaps(&textureObject);
}

AmberGL::SoftwareRenderer::RenderObject::TextureObject* AmberGL::GetTextureObject(uint32_t p_textureUnit)
{
	if (p_textureUnit >= MAX_TEXTURE_UNITS)
	{
		std::cout << "GetTextureObject: Texture unit " << p_textureUnit << " is out of range.\n";
		return nullptr;
	}
	return BoundTextureUnits[p_textureUnit];
}

void AmberGL::GenFrameBuffers(uint32_t p_count, uint32_t* p_frameBuffers)
{
	for (uint32_t i = 0; i < p_count; i++)
	{
		AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject frameBufferObject;
		frameBufferObject.ID = FrameBufferObjectID;
		FrameBufferObjects[FrameBufferObjectID] = frameBufferObject;
		p_frameBuffers[i] = FrameBufferObjectID;
		FrameBufferObjectID++;
	}
}

void AmberGL::BindFrameBuffer(uint32_t p_target, uint32_t p_frameBuffer)
{
	if (p_target != AGL_FRAMEBUFFER)
	{
		std::cout << "BindFrameBuffer: only AGL_FRAMEBUFFER is supported.\n";
		return;
	}

	if (p_frameBuffer != 0 && FrameBufferObjects.find(p_frameBuffer) == FrameBufferObjects.end())
	{
		std::cout << "FrameBuffer " << p_frameBuffer << " not found!\n";
		return;
	}

	if (p_frameBuffer == 0)
	{
		if (CurrentFrameBuffer != 0)
		{
			AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject& frameBufferObject = FrameBufferObjects[CurrentFrameBuffer];

			if (frameBufferObject.DepthBuffer && frameBufferObject.AttachedTexture)
			{
				//TODO: not the good place to update the attached texture
				frameBufferObject.DepthBuffer->UpdateAttachedTextureObject(frameBufferObject.AttachedTexture);
			}
		}

		ActiveDepthBuffer = BackBuffer.DepthBuffer;
		ActiveColorBuffer = BackBuffer.ColorBuffer;
		CurrentFrameBuffer = 0;

		RenderContext.FrameBufferObject = &BackBuffer;
	}
	else
	{
		CurrentFrameBuffer = p_frameBuffer;
		AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject& frameBufferObject = FrameBufferObjects[CurrentFrameBuffer];

		ActiveDepthBuffer = frameBufferObject.DepthBuffer;
		ActiveColorBuffer = frameBufferObject.Attachment == AGL_COLOR_ATTACHMENT ? frameBufferObject.ColorBuffer : nullptr;

		RenderContext.FrameBufferObject = &frameBufferObject;
	}
}

void AmberGL::FrameBufferTexture2D(uint32_t p_target, uint16_t p_attachment, uint32_t p_textarget, uint32_t p_texture, int p_level)
{
	if (p_target != AGL_FRAMEBUFFER)
	{
		std::cout << "FrameBufferTexture2D: only AGL_FRAMEBUFFER is supported.\n";
		return;
	}

	if (CurrentFrameBuffer == 0)
	{
		std::cout << "No FBO currently bound.\n";
		return;
	}

	if (p_texture != 0 && TextureObjects.find(p_texture) == TextureObjects.end())
	{
		std::cout << "Texture " << p_texture << " not found.\n";
		return;
	}

	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject& frameBufferObject = FrameBufferObjects[CurrentFrameBuffer];

	if (p_texture == 0)
	{
		if (p_attachment == AGL_DEPTH_ATTACHMENT)
		{
			delete frameBufferObject.DepthBuffer;
			frameBufferObject.DepthBuffer = nullptr;
			
			if (frameBufferObject.AttachedTexture && p_attachment == AGL_DEPTH_ATTACHMENT)
				frameBufferObject.AttachedTexture = nullptr;
		}
		else if (p_attachment == AGL_COLOR_ATTACHMENT)
		{
			delete frameBufferObject.ColorBuffer;
			frameBufferObject.ColorBuffer = nullptr;
		}
		return;
	}

	AmberGL::SoftwareRenderer::RenderObject::TextureObject* textureObject = TextureObjects[p_texture];
	
	if (p_attachment == AGL_DEPTH_ATTACHMENT)
	{
		delete frameBufferObject.DepthBuffer;

		frameBufferObject.DepthBuffer = new AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Depth>(textureObject->Width, textureObject->Height);
		frameBufferObject.AttachedTexture = textureObject;
	}
	else if (p_attachment == AGL_COLOR_ATTACHMENT)
	{
		delete frameBufferObject.ColorBuffer;

		frameBufferObject.ColorBuffer = new AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<RGBA8>(textureObject->Width, textureObject->Height);
	}

	frameBufferObject.Attachment = p_attachment;
}

void AmberGL::DrawBuffer(uint32_t p_mode)
{
	//TODO
}

void AmberGL::ReadBuffer(uint32_t p_mode)
{
	//TODO
}

uint32_t* AmberGL::GetFrameBufferData()
{
	return ActiveColorBuffer->Data;
}

uint32_t AmberGL::GetFrameBufferRowSize()
{
	return ActiveColorBuffer->RowSize;
}

void AmberGL::Initialize(uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight)
{
	RenderContext.Viewport.Width = p_rasterizationBufferWidth;
	RenderContext.Viewport.Height = p_rasterizationBufferHeight;

	BackBuffer.ColorBuffer = new AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<RGBA8>(p_rasterizationBufferWidth, p_rasterizationBufferHeight);
	BackBuffer.DepthBuffer = new AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Depth>(p_rasterizationBufferWidth, p_rasterizationBufferHeight);

	ActiveColorBuffer = BackBuffer.ColorBuffer;
	ActiveDepthBuffer = BackBuffer.DepthBuffer;

	RenderContext.FrameBufferObject = &BackBuffer;

	MSAABuffer = new AmberGL::SoftwareRenderer::Buffers::MSAABuffer(p_rasterizationBufferWidth, p_rasterizationBufferHeight);

	InitializeClippingFrustum();
}

void AmberGL::DrawElements(uint16_t p_primitiveMode, uint32_t p_indexCount)
{
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<RGBA8>* originalColorBuffer = ActiveColorBuffer;
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Depth>* originalDepthBuffer = ActiveDepthBuffer;

	if (CurrentFrameBuffer != 0)
	{
		AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject& fbo = FrameBufferObjects[CurrentFrameBuffer];
		ActiveDepthBuffer = fbo.DepthBuffer;
		ActiveColorBuffer = fbo.Attachment == AGL_COLOR_ATTACHMENT ? fbo.ColorBuffer : nullptr;
	}
	else
	{
		ActiveColorBuffer = BackBuffer.ColorBuffer;
		ActiveDepthBuffer = BackBuffer.DepthBuffer;
	}

	AmberGL::SoftwareRenderer::RenderObject::VertexArrayObject* vao = GetBoundVertexArrayObject();
	if (!vao || vao->BoundElementBuffer == 0 || vao->BoundArrayBuffer == 0)
	{
		std::cout << "Invalid VAO or buffers for drawing!\n";

		ActiveColorBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		return;
	}

	auto itIndex = BufferObjects.find(vao->BoundElementBuffer);
	auto itVertex = BufferObjects.find(vao->BoundArrayBuffer);
	if (itIndex == BufferObjects.end() || itVertex == BufferObjects.end())
	{
		std::cout << "Buffers not found!\n";

		ActiveColorBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		return;
	}

	AmberGL::SoftwareRenderer::RenderObject::BufferObject& indexBufferObject = itIndex->second;
	AmberGL::SoftwareRenderer::RenderObject::BufferObject& vertexBuffer = itVertex->second;
	AmberGL::Geometry::Vertex* vertices = reinterpret_cast<AmberGL::Geometry::Vertex*>(vertexBuffer.Data.data());
	uint32_t* indices = reinterpret_cast<uint32_t*>(indexBufferObject.Data.data());

	switch (p_primitiveMode)
	{
	case AGL_TRIANGLES:
		for (size_t i = 0; i + 2 < p_indexCount; i += 3)
		{
			RasterizeTriangle(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
		}
		break;
	case AGL_TRIANGLE_STRIP:
		for (size_t i = 0; i + 2 < p_indexCount; i++)
		{
			if (i % 2 == 0)
			{
				RasterizeTriangle(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
			}
			else
			{
				RasterizeTriangle(vertices[indices[i]], vertices[indices[i + 2]], vertices[indices[i + 1]]);
			}
		}
		break;
	case AGL_LINES:
		for (size_t i = 0; i + 1 < p_indexCount; i += 2)
		{
			ProcessLine(vertices[indices[i]], vertices[indices[i + 1]]);
		}
		break;
	case AGL_POINTS:
		for (size_t i = 0; i < p_indexCount; i++)
		{
			ProcessPoint(vertices[indices[i]]);
		}
		break;

	default:
		//TODO: Log Unsupported primitive mode.
		break;
	}

	if (RenderContext.State & AGL_MULTISAMPLE && ActiveColorBuffer != nullptr)
		ApplyMSAA();

	ActiveColorBuffer = originalColorBuffer;
	ActiveDepthBuffer = originalDepthBuffer;
}

void AmberGL::DrawArrays(uint16_t p_primitiveMode, uint32_t p_first, uint32_t p_count)
{
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<RGBA8>* originalColorBuffer = ActiveColorBuffer;
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Depth>* originalDepthBuffer = ActiveDepthBuffer;

	if (CurrentFrameBuffer != 0)
	{
		AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject& fbo = FrameBufferObjects[CurrentFrameBuffer];
		ActiveDepthBuffer = fbo.DepthBuffer;
		ActiveColorBuffer = fbo.Attachment == AGL_COLOR_ATTACHMENT ? fbo.ColorBuffer : nullptr;
	}
	else
	{
		ActiveColorBuffer = BackBuffer.ColorBuffer;
		ActiveDepthBuffer = BackBuffer.DepthBuffer;
	}

	AmberGL::SoftwareRenderer::RenderObject::VertexArrayObject* vao = GetBoundVertexArrayObject();
	if (!vao || vao->BoundArrayBuffer == 0)
	{
		std::cout << "Invalid VAO or buffers for drawing!\n";

		ActiveColorBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		return;
	}

	auto itVertex = BufferObjects.find(vao->BoundArrayBuffer);
	if (itVertex == BufferObjects.end())
	{
		std::cout << "Vertex buffer not found!\n";

		ActiveColorBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		return;
	}

	AmberGL::SoftwareRenderer::RenderObject::BufferObject& vertexBuffer = itVertex->second;
	AmberGL::Geometry::Vertex* vertices = reinterpret_cast<AmberGL::Geometry::Vertex*>(vertexBuffer.Data.data());

	switch (p_primitiveMode)
	{
	case AGL_TRIANGLES:
		for (uint32_t i = p_first; i + 2 < p_first + p_count; i += 3)
		{
			RasterizeTriangle(vertices[i], vertices[i + 1], vertices[i + 2]);
		}
		break;
	case AGL_TRIANGLE_STRIP:
		for (uint32_t i = p_first; i < p_first + p_count - 2; i++)
		{
			if ((i - p_first) % 2 == 0)
			{
				RasterizeTriangle(vertices[i], vertices[i + 1], vertices[i + 2]);
			}
			else
			{
				RasterizeTriangle(vertices[i + 1], vertices[i], vertices[i + 2]);
			}
		}
		break;
	case AGL_LINES:
		for (uint32_t i = p_first; i + 1 < p_first + p_count; i += 2)
		{
			ProcessLine(vertices[i], vertices[i + 1]);
		}
		break;
	case AGL_POINTS:
		for (uint32_t i = p_first; i < p_first + p_count; i++)
		{
			ProcessPoint(vertices[i]);
		}
		break;
	default:
		std::cout << "Unsupported primitive mode: " << static_cast<int>(p_primitiveMode) << std::endl;
		break;
	}

	if (RenderContext.State & AGL_MULTISAMPLE && ActiveColorBuffer != nullptr)
		ApplyMSAA();

	ActiveColorBuffer = originalColorBuffer;
	ActiveDepthBuffer = originalDepthBuffer;
}

void AmberGL::PointSize(float p_size)
{
	RenderContext.PointSize = p_size;
}

void AmberGL::LineWidth(float p_width)
{
	RenderContext.LineWidth = p_width;
}

void AmberGL::DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, const glm::vec4& p_color)
{
	AmberGL::Geometry::Vertex vertex0(p_point0);
	AmberGL::Geometry::Vertex vertex1(p_point1);

	ProcessLine(vertex0, vertex1);

	if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer != nullptr)
	{
		ApplyMSAA();
	}
}

void AmberGL::DrawPoint(const glm::vec3& p_point0)
{
	AmberGL::Geometry::Vertex vertex0(p_point0);

	ProcessPoint(vertex0);

	if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer != nullptr)
	{
		ApplyMSAA();
	}
}

void AmberGL::DepthFunc(uint16_t p_func)
{
	if (p_func >= AGL_NEVER && p_func <= AGL_ALWAYS)
	{
		RenderContext.DepthFunc = p_func;
	}
	else
	{
		RenderContext.DepthFunc = AGL_LESS;
		//TODO: Log Invalid depth function, falling back to AGL_LESS.
	}
}

uint32_t AmberGL::CreateProgram()
{
	AmberGL::SoftwareRenderer::RenderObject::ProgramObject programObject;
	programObject.ID = ProgramID;
	programObject.Program = nullptr;
	ProgramObjects[ProgramID] = programObject;

	return ProgramID++;
}

void AmberGL::DeleteProgram(uint32_t p_program)
{
	auto it = ProgramObjects.find(p_program);
	if (it == ProgramObjects.end())
	{
		std::cout << "DeleteProgram: Program " << p_program << " not found.\n";
		return;
	}

	ProgramObjects.erase(it);

	if (CurrentProgramObject == p_program)
	{
		CurrentProgramObject = 0;
		RenderContext.Program = nullptr;
	}
}

void AmberGL::UseProgram(uint32_t p_program)
{
	if (p_program == 0)
	{
		CurrentProgramObject = 0;
		RenderContext.Program = nullptr;
		return;
	}

	auto it = ProgramObjects.find(p_program);
	if (it == ProgramObjects.end())
	{
		std::cout << "UseProgram: Program " << p_program << " not found.\n";
		return;
	}

	const AmberGL::SoftwareRenderer::RenderObject::ProgramObject& programObject = it->second;

	if (programObject.Program == nullptr)
	{
		std::cout << "UseProgram: Program " << p_program << " has no shader attached.\n";
		return;
	}

	CurrentProgramObject = p_program;
	RenderContext.Program = programObject.Program;
}

void AmberGL::AttachShader(uint32_t p_program, AmberGL::SoftwareRenderer::Programs::AProgram* p_programInstance)
{
	auto it = ProgramObjects.find(p_program);
	if (it == ProgramObjects.end())
	{
		std::cout << "AttachShader: Program " << p_program << " not found.\n";
		return;
	}

	AmberGL::SoftwareRenderer::RenderObject::ProgramObject& programObject = it->second;
	programObject.Program = p_programInstance;
}

void AmberGL::UseProgram(AmberGL::SoftwareRenderer::Programs::AProgram* p_programInstance)
{
	for (const auto& [id, programObject] : ProgramObjects)
	{
		if (programObject.Program == p_programInstance)
		{
			UseProgram(id);
			return;
		}
	}

	uint32_t programId = CreateProgram();
	AttachShader(programId, p_programInstance);
	UseProgram(programId);
}

void AmberGL::SetSamples(uint8_t p_samples)
{
	RenderContext.Samples = p_samples;
	MSAABuffer->SetSamplesAmount(p_samples);
}

void AmberGL::PolygonMode(uint16_t p_mode)
{
	if (p_mode <= AGL_POINT)
		RenderContext.PolygonMode = p_mode;
	else
		RenderContext.PolygonMode = AGL_FILL;
}

void AmberGL::Enable(uint8_t p_state)
{
	RenderContext.State |= p_state;
}

void AmberGL::Disable(uint8_t p_state)
{
	RenderContext.State &= ~p_state;
}

bool AmberGL::IsEnabled(uint8_t p_capability)
{
	return (RenderContext.State & p_capability) != 0;
}

void AmberGL::CullFace(uint16_t p_face)
{
	if (p_face <= AGL_FRONT_AND_BACK)
		RenderContext.CullFace = p_face;
	else
		RenderContext.CullFace = AGL_BACK;
}

void AmberGL::DepthMask(bool p_flag)
{
	if (p_flag) 
	{
		Enable(AGL_DEPTH_WRITE);
	}
	else 
	{
		Disable(AGL_DEPTH_WRITE);
	}
}

void AmberGL::GetBool(uint16_t p_name, bool* p_params)
{
	if (!p_params) return;

	switch (p_name)
	{
	case AGL_DEPTH_WRITE:
		*p_params = (RenderContext.State & AGL_DEPTH_WRITE) != 0;
		break;
	case AGL_DEPTH_TEST:
		*p_params = (RenderContext.State & AGL_DEPTH_TEST) != 0;
		break;
	case AGL_CULL_FACE:
		*p_params = (RenderContext.State & AGL_CULL_FACE) != 0;
		break;
	default:
		*p_params = false;
		break;
	}
}

void AmberGL::GetInt(uint16_t p_name, int* p_params)
{
	if (!p_params) return;

	switch (p_name)
	{
	case AGL_CULL_FACE:
		*p_params = RenderContext.CullFace;
		break;
	case AGL_FILL:
	case AGL_LINE:
	case AGL_POINT:
		*p_params = RenderContext.PolygonMode;
		break;
	case AGL_VIEWPORT:
		p_params[0] = RenderContext.Viewport.X;
		p_params[1] = RenderContext.Viewport.Y;
		p_params[2] = RenderContext.Viewport.Width;
		p_params[3] = RenderContext.Viewport.Height;
		break;
	default:
		*p_params = -1;
		break;
	}
}

void AmberGL::Terminate()
{
	for (auto& [id, textureObject] : TextureObjects)
	{
		if (textureObject)
		{
			if (textureObject->Mipmaps)
			{
				int maxLevel = 1 + static_cast<int>(std::floor(std::log2(std::max(textureObject->Width, textureObject->Height))));

				for (int i = 0; i < maxLevel; i++)
				{
					delete[] textureObject->Mipmaps[i];
					textureObject->Mipmaps[i] = nullptr;
				}

				delete[] textureObject->Mipmaps;
				textureObject->Mipmaps = nullptr;
			}

			delete[] textureObject->Data8;
			textureObject->Data8 = nullptr;

			delete textureObject;
			textureObject = nullptr;
		}
	}

	for (auto& [id, programObject] : ProgramObjects)
	{
		delete programObject.Program;
		programObject.Program = nullptr;
	}

	TextureObjects.clear();
	ProgramObjects.clear();
	FrameBufferObjects.clear();
	BufferObjects.clear();
	VertexArrayObjects.clear();

	CurrentVertexArrayObject = 0;
	CurrentArrayBuffer = 0;
	CurrentElementBuffer = 0;
	CurrentTexture = 0;
	CurrentProgramObject = 0;
	CurrentFrameBuffer = 0;

	VertexArrayID = 1;
	BufferID = 1;
	TextureID = 1;
	ProgramID = 1;
	FrameBufferObjectID = 1;

	ActiveDepthBuffer = nullptr;

	delete MSAABuffer;
	MSAABuffer = nullptr;

	for (uint32_t i = 0; i < MAX_TEXTURE_UNITS; i++)
	{
		BoundTextureUnits[i] = nullptr;
	}
}

void AmberGL::WindowHint(uint16_t p_name, int p_value)
{
	if (p_name == AGL_SAMPLES)
	{
		SetSamples(p_value);
	}
}

void AmberGL::BlitFrameBuffer(uint32_t p_sourceFrameBuffer, uint32_t p_destinationFrameBuffer, int p_sourceX0, int p_sourceY0, int p_sourceX1, int p_sourceY1, int p_destinationX0, int p_destinationY0, int p_destinationX1, int p_destinationY1, uint16_t p_mask, uint16_t p_filter)
{
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject* sourceFrameBufferObject = nullptr;

	if (p_sourceFrameBuffer == 0)
	{
		sourceFrameBufferObject = &BackBuffer;
	}
	else
	{
		auto it = FrameBufferObjects.find(p_sourceFrameBuffer);

		if (it == FrameBufferObjects.end())
		{
			//TODO: Log source not found.
			return;
		}
		sourceFrameBufferObject = &it->second;
	}

	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject* destinationFrameBufferObject = nullptr;

	if (p_destinationFrameBuffer == 0)
	{
		destinationFrameBufferObject = &BackBuffer;
	}
	else
	{
		auto it = FrameBufferObjects.find(p_destinationFrameBuffer);
		if (it == FrameBufferObjects.end())
		{
			//TODO: Log dest not found.
			return;
		}
		destinationFrameBufferObject = &it->second;
	}

	bool isDepthVisualization = false;

	if (p_mask & AGL_COLOR_BUFFER_BIT && !sourceFrameBufferObject->ColorBuffer && sourceFrameBufferObject->DepthBuffer && destinationFrameBufferObject->ColorBuffer)
	{
		isDepthVisualization = true;
	}

	p_sourceX0 = std::max(0, p_sourceX0);
	p_sourceY0 = std::max(0, p_sourceY0);

	uint32_t sourceWidth = 0;
	uint32_t sourceHeight = 0;

	if (sourceFrameBufferObject->ColorBuffer)
	{
		sourceWidth = sourceFrameBufferObject->ColorBuffer->Width;
		sourceHeight = sourceFrameBufferObject->ColorBuffer->Height;
	}
	else if (sourceFrameBufferObject->DepthBuffer)
	{
		sourceWidth = sourceFrameBufferObject->DepthBuffer->Width;
		sourceHeight = sourceFrameBufferObject->DepthBuffer->Height;
	}
	else
	{
		//TODO: Log source has no color or depth buffer.
		return;
	}

	p_sourceX1 = std::min(p_sourceX1, static_cast<int>(sourceWidth));
	p_sourceY1 = std::min(p_sourceY1, static_cast<int>(sourceHeight));

	p_destinationX0 = std::max(0, p_destinationX0);
	p_destinationY0 = std::max(0, p_destinationY0);

	uint32_t destinationWidth = 0;
	uint32_t destinationHeight = 0;

	if (destinationFrameBufferObject->ColorBuffer)
	{
		destinationWidth = destinationFrameBufferObject->ColorBuffer->Width;
		destinationHeight = destinationFrameBufferObject->ColorBuffer->Height;
	}
	else if (destinationFrameBufferObject->DepthBuffer)
	{
		destinationWidth = destinationFrameBufferObject->DepthBuffer->Width;
		destinationHeight = destinationFrameBufferObject->DepthBuffer->Height;
	}
	else
	{
		//TODO: Log dest has no color or depth buffer.
		return;
	}

	p_destinationX1 = std::min(p_destinationX1, static_cast<int>(destinationWidth));
	p_destinationY1 = std::min(p_destinationY1, static_cast<int>(destinationHeight));

	int sourceRegionWidth = p_sourceX1 - p_sourceX0;
	int sourceRegionHeight = p_sourceY1 - p_sourceY0;
	int destinationRegionWidth = p_destinationX1 - p_destinationX0;
	int destinationRegionHeight = p_destinationY1 - p_destinationY0;

	if (sourceRegionWidth <= 0 || sourceRegionHeight <= 0 || destinationRegionWidth <= 0 || destinationRegionHeight <= 0)
	{
		//TODO: Log invalid region.
		return;
	}

	float scaleX = static_cast<float>(sourceRegionWidth) / destinationRegionWidth;
	float scaleY = static_cast<float>(sourceRegionHeight) / destinationRegionHeight;

	if ((p_mask & AGL_COLOR_BUFFER_BIT && sourceFrameBufferObject->ColorBuffer && destinationFrameBufferObject->ColorBuffer) || isDepthVisualization)
	{
		for (int y = 0; y < destinationRegionHeight; y++)
		{
			for (int x = 0; x < destinationRegionWidth; x++)
			{
				int destinationX = p_destinationX0 + x;
				int destinationY = p_destinationY0 + y;

				if (p_filter == AGL_NEAREST)
				{
					int sourceX = p_sourceX0 + static_cast<int>(x * scaleX);
					int sourceY = p_sourceY0 + static_cast<int>(y * scaleY);

					if (isDepthVisualization)
					{
						float depth = sourceFrameBufferObject->DepthBuffer->GetPixel(sourceX, sourceY);
						depth = 1.0f - depth;
						glm::vec4 depthColor(depth, depth, depth, 1.0f);
						destinationFrameBufferObject->ColorBuffer->SetPixel(destinationX, destinationY, PackColor(depthColor));
					}
					else
					{
						uint32_t color = sourceFrameBufferObject->ColorBuffer->GetPixel(sourceX, sourceY);
						destinationFrameBufferObject->ColorBuffer->SetPixel(destinationX, destinationY, color);
					}
				}
				else
				{
					float sourceFloatX = p_sourceX0 + x * scaleX;
					float sourceFloatY = p_sourceY0 + y * scaleY;

					int sourceX1 = static_cast<int>(sourceFloatX);
					int sourceY1 = static_cast<int>(sourceFloatY);
					int sourceX2 = std::min(sourceX1 + 1, p_sourceX1 - 1);
					int sourceY2 = std::min(sourceY1 + 1, p_sourceY1 - 1);

					float fracX = sourceFloatX - sourceX1;
					float fracY = sourceFloatY - sourceY1;

					if (isDepthVisualization)
					{
						float depth11 = sourceFrameBufferObject->DepthBuffer->GetPixel(sourceX1, sourceY1);
						float depth12 = sourceFrameBufferObject->DepthBuffer->GetPixel(sourceX1, sourceY2);
						float depth21 = sourceFrameBufferObject->DepthBuffer->GetPixel(sourceX2, sourceY1);
						float depth22 = sourceFrameBufferObject->DepthBuffer->GetPixel(sourceX2, sourceY2);

						float depthTop = glm::mix(depth11, depth21, fracX);
						float depthBottom = glm::mix(depth12, depth22, fracX);
						float finalDepth = glm::mix(depthTop, depthBottom, fracY);

						finalDepth = 1.0f - finalDepth;
						glm::vec4 depthColor(finalDepth, finalDepth, finalDepth, 1.0f);
						destinationFrameBufferObject->ColorBuffer->SetPixel(destinationX, destinationY, PackColor(depthColor));
					}
					else
					{
						glm::vec4 color11 = UnpackColor(sourceFrameBufferObject->ColorBuffer->GetPixel(sourceX1, sourceY1));
						glm::vec4 color12 = UnpackColor(sourceFrameBufferObject->ColorBuffer->GetPixel(sourceX1, sourceY2));
						glm::vec4 color21 = UnpackColor(sourceFrameBufferObject->ColorBuffer->GetPixel(sourceX2, sourceY1));
						glm::vec4 color22 = UnpackColor(sourceFrameBufferObject->ColorBuffer->GetPixel(sourceX2, sourceY2));

						glm::vec4 colorTop = glm::mix(color11, color21, fracX);
						glm::vec4 colorBottom = glm::mix(color12, color22, fracX);
						glm::vec4 finalColor = glm::mix(colorTop, colorBottom, fracY);

						destinationFrameBufferObject->ColorBuffer->SetPixel(destinationX, destinationY, PackColor(finalColor));
					}
				}
			}
		}
	}

	if (p_mask & AGL_DEPTH_BUFFER_BIT && sourceFrameBufferObject->DepthBuffer && destinationFrameBufferObject->DepthBuffer)
	{
		for (int y = 0; y < destinationRegionHeight; ++y)
		{
			for (int x = 0; x < destinationRegionWidth; ++x)
			{
				int destinationX = p_destinationX0 + x;
				int destinationY = p_destinationY0 + y;

				if (p_filter == AGL_NEAREST)
				{
					int sourceX = p_sourceX0 + static_cast<int>(x * scaleX);
					int sourceY = p_sourceY0 + static_cast<int>(y * scaleY);

					float depth = sourceFrameBufferObject->DepthBuffer->GetPixel(sourceX, sourceY);
					destinationFrameBufferObject->DepthBuffer->SetPixel(destinationX, destinationY, depth);
				}
				else
				{
					float sourceFloatX = p_sourceX0 + x * scaleX;
					float sourceFloatY = p_sourceY0 + y * scaleY;

					int sourceX1 = static_cast<int>(sourceFloatX);
					int sourceY1 = static_cast<int>(sourceFloatY);
					int sourceX2 = std::min(sourceX1 + 1, p_sourceX1 - 1);
					int sourceY2 = std::min(sourceY1 + 1, p_sourceY1 - 1);

					float fracX = sourceFloatX - sourceX1;
					float fracY = sourceFloatY - sourceY1;

					float depth11 = sourceFrameBufferObject->DepthBuffer->GetPixel(sourceX1, sourceY1);
					float depth12 = sourceFrameBufferObject->DepthBuffer->GetPixel(sourceX1, sourceY2);
					float depth21 = sourceFrameBufferObject->DepthBuffer->GetPixel(sourceX2, sourceY1);
					float depth22 = sourceFrameBufferObject->DepthBuffer->GetPixel(sourceX2, sourceY2);

					float depthTop = glm::mix(depth11, depth21, fracX);
					float depthBottom = glm::mix(depth12, depth22, fracX);
					float finalDepth = glm::mix(depthTop, depthBottom, fracY);

					destinationFrameBufferObject->DepthBuffer->SetPixel(destinationX, destinationY, finalDepth);
				}
			}
		}
	}
}

void AmberGL::ClearColor(float p_red, float p_green, float p_blue, float p_alpha)
{
	if (ActiveColorBuffer)
		ActiveColorBuffer->SetColor(p_red, p_green, p_blue, p_alpha);

	if (RenderContext.State & AGL_MULTISAMPLE)
		MSAABuffer->SetColor(p_red, p_green, p_blue, p_alpha);
}

void AmberGL::Clear(uint8_t p_flags)
{
	if (p_flags & AGL_COLOR_BUFFER_BIT)
	{
		if (ActiveColorBuffer)
			ActiveColorBuffer->Clear();
	}

	if (p_flags & AGL_DEPTH_BUFFER_BIT)
	{
		ActiveDepthBuffer->Clear();
	}

	if (RenderContext.State & AGL_MULTISAMPLE)
		MSAABuffer->Clear();
}

void AmberGL::Viewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height)
{
	RenderContext.Viewport.X = p_x;
	RenderContext.Viewport.Y = p_y;
	RenderContext.Viewport.Width = p_width;
	RenderContext.Viewport.Height = p_height;
}