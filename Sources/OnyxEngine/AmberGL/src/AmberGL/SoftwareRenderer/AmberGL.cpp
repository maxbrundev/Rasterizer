#include "AmberGL/SoftwareRenderer/AmberGL.h"

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <unordered_set>

#define GLM_ENABLE_EXPERIMENTAL
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
#include "AmberGL/SoftwareRenderer/Programs/AProgram.h"

#include "AmberGL/SoftwareRenderer/RenderObject/ProgramObject.h"
#include "AmberGL/SoftwareRenderer/RenderObject/RenderContext.h"
#include "AmberGL/SoftwareRenderer/RenderObject/UniformBufferObject.h"
#include "AmberGL/SoftwareRenderer/RenderObject/VertexArrayObject.h"
#include "AmberGL/SoftwareRenderer/RenderObject/VertexBufferObject.h"

/////////////////////////////////////////////////////////////////

// This file contains way too many functions. I’m aware of that, of course lmao, but it's on purpose:
// 1) Anonymous rasterization-related functions, so in any case there will be a lot of functions and variables declared somewhere in a .cpp.
// 2) This is a personal learning project. I accumulate everything here. I have duplicated functions for tile-based / non-tile-based rendering, 
// lines with width / classic lines, etc...
// The main goal is personal learning, not shipping software or a generic engine either.

// This is NOT how I’d organize production code, but it’s perfect for rapid iteration and comparing different techniques side-by-side.

/////////////////////////////////////////////////////////////////

#define TILE_BASED

constexpr uint8_t TILE_SIZE = 16;
constexpr uint8_t TILE_MARGIN = 1;

thread_local AmberGL::Geometry::BoundingBox2D TileBoundingBox = { {0, 0}, {INT_MAX, INT_MAX} };
thread_local AmberGL::Geometry::BoundingBox2D WriteBoundingBox = { {0, 0}, {INT_MAX, INT_MAX} };
thread_local bool IsTileRenderPixel = false;

struct ClippedPrimitive
{
	AmberGL::Geometry::Polygon ClippedPolygon;
	std::vector<glm::vec2> ScreenSpaceVertices;
	AmberGL::Geometry::BoundingBox2D BoundingBox;
	uint32_t Index;
};

uint32_t TilesX;
uint32_t TilesY;
uint32_t TileCount;

std::vector<std::vector<uint32_t>> tileTriangleLists;
std::vector<AmberGL::Geometry::Polygon> ClippedPrimitives;
std::vector<std::future<void>> TileJobs;

static bool IsDrawDebugTile = false;

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

	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject FrontBuffer;
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject BackBuffer;

	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<RGBA8>* ActiveColorBuffer = nullptr;
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Depth>* ActiveDepthBuffer = nullptr;
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Stencil>* ActiveStencilBuffer = nullptr;

	std::unordered_map<uint32_t, AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject> FrameBufferObjects;
	uint32_t FrameBufferObjectID = 1;
	uint32_t CurrentFrameBuffer = 0;

	std::unordered_map<uint32_t, AmberGL::SoftwareRenderer::RenderObject::ProgramObject> ProgramObjects;
	uint32_t ProgramID = 1;
	uint32_t CurrentProgramObject = 0;

	uint32_t CurrentReadBuffer = AGL_BACK;

	std::unordered_map<uint32_t, AmberGL::SoftwareRenderer::RenderObject::UniformBufferObject> UniformBufferObjects;
	std::vector<uint32_t> uniformBufferObjectBindings;
	uint32_t CurrentUniformBuffer = 0;
	uint32_t UniformBufferID = 1;
}

thread_local std::unique_ptr<AmberGL::SoftwareRenderer::Programs::AProgram> Program;
thread_local const AmberGL::SoftwareRenderer::Programs::AProgram* CurrentProgram = nullptr;

//TODO: ehhh yeah I copy entire program... will make mutable data as context later to have a thread safe type shi program class
AmberGL::SoftwareRenderer::Programs::AProgram& GetThreadLocalShader()
{
	if (!RenderContext.Program)
		throw std::runtime_error("RenderContext.Program is null");

	if (!Program || RenderContext.Program != CurrentProgram) 
	{
		Program = RenderContext.Program->clone();

		if (!Program)
			throw std::runtime_error("Shader clone failed");

		CurrentProgram = RenderContext.Program;
	}
	return *Program;
}

void InitializeClippingFrustum();

glm::vec3 ComputeScreenSpaceCoordinate(const glm::vec4& p_vertexWorldPosition);
glm::vec2 ComputeNormalizedDeviceCoordinate(const glm::vec3& p_vertexScreenSpacePosition);
glm::vec2 ComputeRasterSpaceCoordinate(glm::vec2 p_vertexNormalizedPosition);

void RasterizeTriangle(const AmberGL::Geometry::Vertex& p_vertex0, const AmberGL::Geometry::Vertex& p_vertex1, const AmberGL::Geometry::Vertex& p_vertex2);

void TransformAndRasterizeVertices(const std::array<glm::vec4, 3>& p_processedVertices);

void ComputeFragments(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices);
void ComputeFragmentsTile(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices);
void SetFragment(uint32_t p_x, uint32_t p_y, float p_depth, float w0, float w1, float w2, const glm::vec2& p_dfdx, const glm::vec2& p_dfdy, const glm::vec3& p_barycentricCoords, AmberGL::SoftwareRenderer::Programs::AProgram& p_program);
void SetSampleFragment(const AmberGL::Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, float p_sampleX, float p_sampleY, uint8_t p_sampleIndex, const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec2& p_dfdx, const glm::vec2& p_dfdy, AmberGL::SoftwareRenderer::Programs::AProgram& p_program);

void ClipAgainstPlane(AmberGL::Geometry::Polygon& p_polygon, const AmberGL::Geometry::Plane& p_plane);

void ProcessPoint(const AmberGL::Geometry::Vertex& p_vertex);
void RasterizePoint(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec4& p_point);
void RasterizePointTile(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec4& p_point);
void RasterizePoint(const AmberGL::Geometry::Point& p_point);

void ProcessLine(const AmberGL::Geometry::Vertex& p_vertex0, const AmberGL::Geometry::Vertex& p_vertex1);
void RasterizeLineDirectly(const std::array<glm::vec4, 2>& p_transformedVertices);
void RasterizeLine(const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec4& p_start, const glm::vec4& p_end);
void RasterizeLineTile(const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec4& p_start, const glm::vec4& p_end);

void RasterizeTriangleWireframe(const std::array<glm::vec4, 3>& p_transformedVertices);
void RasterizeTriangleWireframeTile(const std::array<glm::vec4, 3>& p_transformedVertices);
void RasterizeTrianglePoints(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices);
void RasterizeTrianglePointsTile(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices);

bool DepthTest(float fragmentDepth, float bufferDepth);

//TODO: One MSAA per framebuffer
AmberGL::SoftwareRenderer::Buffers::MSAABuffer* MSAABuffer;
std::array<AmberGL::Geometry::Plane, 6> ClippingFrustum;

uint32_t PackColor(const glm::vec4& p_normalizedColor)
{
	return static_cast<uint8_t>(p_normalizedColor.x * 255.0f) << 24 |
		static_cast<uint8_t>(p_normalizedColor.y * 255.0f) << 16 |
		static_cast<uint8_t>(p_normalizedColor.z * 255.0f) << 8 |
		static_cast<uint8_t>(p_normalizedColor.w * 255.0f);
}

glm::vec4 UnpackColor(uint32_t p_packedColor)
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

bool StencilTestFunc(uint8_t p_stencilRef, uint8_t p_stencilBufferValue, uint32_t p_mask, uint16_t p_func)
{
	uint8_t maskedRef = p_stencilRef & p_mask;
	uint8_t maskedBuffer = p_stencilBufferValue & p_mask;

	switch (p_func)
	{
	case AGL_NEVER:    return false;                     break;
	case AGL_LESS:     return maskedRef < maskedBuffer;  break;
	case AGL_EQUAL:    return maskedRef == maskedBuffer; break;
	case AGL_LEQUAL:   return maskedRef <= maskedBuffer; break;
	case AGL_GREATER:  return maskedRef > maskedBuffer;  break;
	case AGL_NOTEQUAL: return maskedRef != maskedBuffer; break;
	case AGL_GEQUAL:   return maskedRef >= maskedBuffer; break;
	case AGL_ALWAYS:
	default:           return true;                      break;
	}
}

uint8_t ApplyStencilOp(uint8_t p_currentValue, uint16_t p_op)
{
	switch (p_op)
	{
	case AGL_KEEP:      return p_currentValue;                                   break;
	case AGL_ZERO:      return 0;                                                break;
	case AGL_REPLACE:   return static_cast<uint8_t>(RenderContext.StencilRef);   break;
	case AGL_INCR:      return p_currentValue == 255 ? 255 : p_currentValue + 1; break;
	case AGL_INCR_WRAP: return p_currentValue + 1;                               break;
	case AGL_DECR:      return (p_currentValue == 0) ? 0 : p_currentValue - 1;   break;
	case AGL_DECR_WRAP: return p_currentValue - 1;                               break;
	case AGL_INVERT:    return ~p_currentValue;                                  break;
	default:            return p_currentValue;                                   break;
	}
}

void UpdateStencilBuffer(uint32_t p_x, uint32_t p_y, bool p_isStencilPassed, bool p_isDepthPassed)
{
	uint8_t currentStencilValue = ActiveStencilBuffer->GetPixel(p_x, p_y);
	uint8_t newStencilValue;

	if (!p_isStencilPassed)
	{
		newStencilValue = ApplyStencilOp(currentStencilValue, RenderContext.StencilFail);
	}
	else if (!p_isDepthPassed)
	{
		newStencilValue = ApplyStencilOp(currentStencilValue, RenderContext.StencilPassDepthFail);
	}
	else
	{
		newStencilValue = ApplyStencilOp(currentStencilValue, RenderContext.StencilPassDepthPass);
	}

	newStencilValue = (newStencilValue & RenderContext.StencilWriteMask) | (currentStencilValue & ~RenderContext.StencilWriteMask);
	ActiveStencilBuffer->SetPixel(p_x, p_y, newStencilValue);
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

void TransformAndRasterizeVertices(const std::array<glm::vec4, 3>& p_processedVertices)
{
	glm::vec3 vertexScreenPosition0 = ComputeScreenSpaceCoordinate(p_processedVertices[0]);
	glm::vec3 vertexScreenPosition1 = ComputeScreenSpaceCoordinate(p_processedVertices[1]);
	glm::vec3 vertexScreenPosition2 = ComputeScreenSpaceCoordinate(p_processedVertices[2]);

	glm::vec2 vertexNormalizedPosition0 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition0);
	glm::vec2 vertexNormalizedPosition1 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition1);
	glm::vec2 vertexNormalizedPosition2 = ComputeNormalizedDeviceCoordinate(vertexScreenPosition2);

	glm::vec2 vertexRasterPosition0 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition0);
	glm::vec2 vertexRasterPosition1 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition1);
	glm::vec2 vertexRasterPosition2 = ComputeRasterSpaceCoordinate(vertexNormalizedPosition2);

	std::array<glm::vec4, 3> transformedVertices{ p_processedVertices[0], p_processedVertices[1], p_processedVertices[2] };

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

#ifdef TILE_BASED
	switch (RenderContext.PolygonMode)
	{
	case AGL_FILL:
		ComputeFragmentsTile(triangle, transformedVertices);
		break;
	case AGL_LINE:
		RasterizeTriangleWireframeTile(transformedVertices);
		break;
	case AGL_POINT:
		RasterizeTrianglePointsTile(triangle, transformedVertices);
		break;
	}
#else
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
#endif
}

void SetFragment(uint32_t p_x, uint32_t p_y, float p_depth, float w0, float w1, float w2, const glm::vec2& p_dfdx, const glm::vec2& p_dfdy, const glm::vec3& p_barycentricCoords, AmberGL::SoftwareRenderer::Programs::AProgram& p_program)
{
	if (RenderContext.FrameBufferObject->ColorBuffer != nullptr && RenderContext.State & AGL_COLOR_WRITE)
	{
		p_program.ProcessInterpolation(p_barycentricCoords, w0, w1, w2);
		p_program.SetDerivatives(p_dfdx, p_dfdy);

		const glm::vec4 color = p_program.ProcessFragment();

		const uint32_t existingColor = RenderContext.FrameBufferObject->ColorBuffer->GetPixel(p_x, p_y);

		RenderContext.FrameBufferObject->ColorBuffer->SetPixel(p_x, p_y, PackColor(glm::mix(UnpackColor(existingColor), color, color.w)));
	}

	if (RenderContext.State & AGL_DEPTH_WRITE)
	{
		RenderContext.FrameBufferObject->DepthBuffer->SetPixel(p_x, p_y, p_depth);
	}

	if (RenderContext.State & AGL_STENCIL_TEST)
	{
		UpdateStencilBuffer(p_x, p_y, true, true);
	}
}

void SetSampleFragment(const AmberGL::Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, float p_sampleX, float p_sampleY, uint8_t p_sampleIndex, const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec2& p_dfdx, const glm::vec2& p_dfdy, AmberGL::SoftwareRenderer::Programs::AProgram& p_program)
{
	const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ p_sampleX, p_sampleY });

	if (barycentricCoords.x >= 0.0f && barycentricCoords.y >= 0.0f && barycentricCoords.x + barycentricCoords.y <= 1.0f)
	{
		float depth = p_transformedVertices[0].z * barycentricCoords.x + p_transformedVertices[1].z * barycentricCoords.y + p_transformedVertices[2].z * barycentricCoords.z;
		
		depth = depth * 0.5f + 0.5f;
		
		if (depth < 0.0f || depth > 1.0f)
			return;

		if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(p_x, p_y)))
		{
			if (RenderContext.State & AGL_STENCIL_TEST)
			{
				if (ActiveStencilBuffer)
				{
					uint8_t stencilValue = ActiveStencilBuffer->GetPixel(p_x, p_y);
					bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);
		
					UpdateStencilBuffer(p_x, p_y, isStencilPassed, false);
				}
		
			}
		
			return;
		}
		
		if (ActiveStencilBuffer)
		{
			uint8_t stencilValue = ActiveStencilBuffer->GetPixel(p_x, p_y);
			if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
			{
				UpdateStencilBuffer(p_x, p_y, false, true);
				return;
			}
		}

		if (RenderContext.State & AGL_STENCIL_TEST)
		{
			UpdateStencilBuffer(p_x, p_y, true, true);
		}

		p_program.ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

		p_program.SetDerivatives(p_dfdx, p_dfdy);

		const glm::vec4 color = p_program.ProcessFragment();

		MSAABuffer->SetPixelSample(p_x, p_y, p_sampleIndex, PackColor(color), depth, RenderContext.State & AGL_DEPTH_TEST);
	}
}

void ComputeFragments(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices)
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
			glm::vec2 texCoords[2][2] = {};
			bool pixelValid[2][2] = {};
			float pixelsDepth[2][2] = {};
			glm::vec3 pixelBary[2][2] = {};
			bool isAnyPixelValid = false;

			for (int dy = 0; dy < 2; dy++)
			{
				for (int dx = 0; dx < 2; dx++)
				{
					uint32_t px = x + dx;
					uint32_t py = y + dy;

					const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ px, py });

					RenderContext.Program->ComputeInterpolatedReciprocal(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);
					RenderContext.Program->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

					texCoords[dy][dx] = RenderContext.Program->GetVaryingAs<glm::vec2>("v_TexCoords");

					if (px >= xMax || py >= yMax)
						continue;

					if (barycentricCoords.x < 0.0f || barycentricCoords.y < 0.0f || barycentricCoords.x + barycentricCoords.y > 1.0f)
						continue;

					float depth = p_transformedVertices[0].z * barycentricCoords.x + p_transformedVertices[1].z * barycentricCoords.y + p_transformedVertices[2].z * barycentricCoords.z;

					depth = depth * 0.5f + 0.5f;

					if (depth < 0.0f || depth > 1.0f)
						continue;

					if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(px, py)))
					{
						if (RenderContext.State & AGL_STENCIL_TEST)
						{
							if (ActiveStencilBuffer)
							{
								uint8_t stencilValue = ActiveStencilBuffer->GetPixel(px, py);
								bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);

								UpdateStencilBuffer(px, py, isStencilPassed, false);
							}
						}
						continue;
					}

					if (ActiveStencilBuffer)
					{
						uint8_t stencilValue = ActiveStencilBuffer->GetPixel(px, py);
						if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
						{
							UpdateStencilBuffer(px, py, false, true);
							continue;
						}
					}

					isAnyPixelValid = true;
					pixelsDepth[dy][dx] = depth;
					pixelValid[dy][dx] = true;
					pixelBary[dy][dx] = barycentricCoords;
				}
			}

			if (!isAnyPixelValid && (!(RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer != nullptr)))
				continue;

			glm::vec2 dfdx = texCoords[0][1] - texCoords[0][0];
			glm::vec2 dfdy = texCoords[1][0] - texCoords[0][0];

			for (int dy = 0; dy < 2; dy++)
			{
				for (int dx = 0; dx < 2; dx++)
				{
					if (!pixelValid[dy][dx] && (!(RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer != nullptr)))
						continue;

					uint32_t px = x + dx;
					uint32_t py = y + dy;

					if (px >= xMax || py >= yMax)
						continue;

					if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer != nullptr)
					{
						uint8_t gridSize = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));
						uint8_t sampleCount = 0;
						uint32_t color = 0;
						bool isProgramProcessed = false;

						for (uint8_t j = 0; j < gridSize; j++)
						{
							for (uint8_t i = 0; i < gridSize; i++)
							{
								if (sampleCount >= RenderContext.Samples)
									break;

								const float samplePosX = px + (i + 0.5f) / gridSize;
								const float samplePosY = py + (j + 0.5f) / gridSize;

								const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ samplePosX, samplePosY });

								if (barycentricCoords.x >= 0.0f && barycentricCoords.y >= 0.0f && barycentricCoords.x + barycentricCoords.y <= 1.0f)
								{
									float depth = p_transformedVertices[0].z * barycentricCoords.x + p_transformedVertices[1].z * barycentricCoords.y + p_transformedVertices[2].z * barycentricCoords.z;

									depth = depth * 0.5f + 0.5f;

									if (depth < 0.0f || depth > 1.0f)
										continue;

									if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(px, py)))
									{
										if (RenderContext.State & AGL_STENCIL_TEST)
										{
											if (ActiveStencilBuffer)
											{
												uint8_t stencilValue = ActiveStencilBuffer->GetPixel(px, py);
												bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);

												UpdateStencilBuffer(px, py, isStencilPassed, false);
											}
										}
										continue;
									}

									if (ActiveStencilBuffer)
									{
										uint8_t stencilValue = ActiveStencilBuffer->GetPixel(px, py);
										if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
										{
											UpdateStencilBuffer(px, py, false, true);
											continue;
										}
									}

									if (RenderContext.State & AGL_STENCIL_TEST)
									{
										UpdateStencilBuffer(px, py, true, true);
									}

									if (!isProgramProcessed && RenderContext.State & AGL_COLOR_WRITE)
									{
										RenderContext.Program->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

										RenderContext.Program->SetDerivatives(dfdx, dfdy);

										color = PackColor(RenderContext.Program->ProcessFragment());

										isProgramProcessed = true;
									}

									if (RenderContext.State& AGL_COLOR_WRITE)
									{
										MSAABuffer->SetPixelSample(px, py, sampleCount, color, depth, RenderContext.State & AGL_DEPTH_TEST);
									}
								}

								sampleCount++;
							}
						}
					}
					else
					{
						SetFragment(px, py, pixelsDepth[dy][dx], p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w, dfdx, dfdy, pixelBary[dy][dx], *RenderContext.Program);
					}
				}
			}
		}
	}
}

void ComputeFragmentsTile(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices)
{
	uint32_t frameBufferWidth = RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Width : RenderContext.FrameBufferObject->DepthBuffer->Width;
	uint32_t frameBufferHeight = RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Height : RenderContext.FrameBufferObject->DepthBuffer->Height;

	int safeWriteMaxX = std::min(WriteBoundingBox.Max.x, static_cast<int>(frameBufferWidth));
	int safeWriteMaxY = std::min(WriteBoundingBox.Max.y, static_cast<int>(frameBufferHeight));

	const int xMin = std::max(p_triangle.BoundingBox2D.Min.x, WriteBoundingBox.Min.x);
	const int yMin = std::max(p_triangle.BoundingBox2D.Min.y, WriteBoundingBox.Min.y);
	const int xMax = std::min(p_triangle.BoundingBox2D.Max.x, safeWriteMaxX);
	const int yMax = std::min(p_triangle.BoundingBox2D.Max.y, safeWriteMaxY);

	if (xMax <= xMin || yMax <= yMin)
		return;

	AmberGL::SoftwareRenderer::Programs::AProgram& program = GetThreadLocalShader();

	const float w0 = p_transformedVertices[0].w;
	const float w1 = p_transformedVertices[1].w;
	const float w2 = p_transformedVertices[2].w;

	for (uint32_t y = yMin; y < yMax; y += 2)
	{
		for (uint32_t x = xMin; x < xMax; x += 2)
		{
			glm::vec2 texCoords[2][2] = {};
			bool pixelValid[2][2] = {};
			float pixelDepth[2][2] = {};
			glm::vec3 pixelBary[2][2] = {};
			bool quadTouched = false;

			for (int dy = 0; dy < 2; dy++)
			{
				for (int dx = 0; dx < 2; dx++)
				{
					uint32_t px = x + dx;
					uint32_t py = y + dy;

					glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ float(px), float(py) });

					program.ComputeInterpolatedReciprocal(barycentricCoords, w0, w1, w2);
					program.ProcessInterpolation(barycentricCoords, w0, w1, w2);
					texCoords[dy][dx] = program.GetVaryingAs<glm::vec2>("v_TexCoords");

					if (px >= xMax || py >= yMax)
						continue;

					if (barycentricCoords.x < 0.0f || barycentricCoords.y < 0.0f || barycentricCoords.x + barycentricCoords.y > 1.0f)
						continue;

					float depth = p_transformedVertices[0].z * barycentricCoords.x + p_transformedVertices[1].z * barycentricCoords.y + p_transformedVertices[2].z * barycentricCoords.z;

					depth = depth * 0.5f + 0.5f;

					if (depth < 0.0f || depth > 1.0f)
						continue;

					if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(px, py)))
					{
						if (RenderContext.State & AGL_STENCIL_TEST)
						{
							if (ActiveStencilBuffer)
							{
								uint8_t stencilValue = ActiveStencilBuffer->GetPixel(px, py);
								bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);
						
								UpdateStencilBuffer(px, py, isStencilPassed, false);
							}
								
						}

						continue;
					}

					if (ActiveStencilBuffer)
					{
						uint8_t stencilValue = ActiveStencilBuffer->GetPixel(px, py);
						if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
						{
							UpdateStencilBuffer(px, py, false, true);
							continue;
						}
					}

					quadTouched = true;
					pixelValid[dy][dx] = true;
					pixelDepth[dy][dx] = depth;
					pixelBary[dy][dx] = barycentricCoords;
				}
			}

			if (!quadTouched && (!(RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer != nullptr)))
				continue;

			glm::vec2 dfdx = texCoords[0][1] - texCoords[0][0];
			glm::vec2 dfdy = texCoords[1][0] - texCoords[0][0];

			for (int dy = 0; dy < 2; dy++)
			{
				for (int dx = 0; dx < 2; dx++)
				{
					if (!pixelValid[dy][dx] && (!(RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer != nullptr)))
						continue;

					uint32_t px = x + dx;
					uint32_t py = y + dy;

					if (px < (uint32_t)WriteBoundingBox.Min.x || px >= (uint32_t)WriteBoundingBox.Max.x ||
						py < (uint32_t)WriteBoundingBox.Min.y || py >= (uint32_t)WriteBoundingBox.Max.y)
						continue;

					IsTileRenderPixel = true;

					if ((RenderContext.State & AGL_MULTISAMPLE) && RenderContext.FrameBufferObject->ColorBuffer != nullptr)
					{
						uint8_t gridSize = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));
						uint8_t sampleCount = 0;
						uint32_t color = 0;
						bool isProgramProcessed = false;

						for (uint8_t j = 0; j < gridSize; j++)
						{
							for (uint8_t i = 0; i < gridSize; i++)
							{
								if (sampleCount >= RenderContext.Samples)
									break;

								float sampleX = px + (i + 0.5f) / gridSize;
								float sampleY = py + (j + 0.5f) / gridSize;

								/////////////////////////

								const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ sampleX, sampleY });

								if (barycentricCoords.x >= 0.0f && barycentricCoords.y >= 0.0f && barycentricCoords.x + barycentricCoords.y <= 1.0f)
								{
									float depth = p_transformedVertices[0].z * barycentricCoords.x + p_transformedVertices[1].z * barycentricCoords.y + p_transformedVertices[2].z * barycentricCoords.z;

									depth = depth * 0.5f + 0.5f;

									if (depth < 0.0f || depth > 1.0f)
										continue;

									if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(px, py)))
									{
										if (RenderContext.State & AGL_STENCIL_TEST)
										{
											if (ActiveStencilBuffer)
											{
												uint8_t stencilValue = ActiveStencilBuffer->GetPixel(px, py);
												bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);
									
												UpdateStencilBuffer(px, py, isStencilPassed, false);
											}
									
										}
									
										continue;
									}
									
									if (ActiveStencilBuffer)
									{
										uint8_t stencilValue = ActiveStencilBuffer->GetPixel(px, py);
										if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
										{
											UpdateStencilBuffer(px, py, false, true);
											continue;
										}
									}

									if (RenderContext.State & AGL_STENCIL_TEST)
									{
										UpdateStencilBuffer(px, py, true, true);
									}

									if (!isProgramProcessed && RenderContext.State & AGL_COLOR_WRITE)
									{
										program.ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

										program.SetDerivatives(dfdx, dfdy);

										color = PackColor(program.ProcessFragment());

										isProgramProcessed = true;
									}

									if (RenderContext.State & AGL_COLOR_WRITE)
									{
										MSAABuffer->SetPixelSample(px, py, sampleCount, color, depth, RenderContext.State & AGL_DEPTH_TEST);
									}
								}

								/////////////////////////
								//SetSampleFragment(p_triangle, px, py, sampleX, sampleY, sampleCount, p_transformedVertices, dfdx, dfdy, program);
								sampleCount++;
							}
						}
					}
					else
					{
						if (pixelValid[dy][dx])
						{
							SetFragment(px, py, pixelDepth[dy][dx], w0, w1, w2, dfdx, dfdy, pixelBary[dy][dx], program);
						}
					}
				}
			}
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

	float depth = p_transformedVertices[0].z * centerBarycentricCoords.x
	+ p_transformedVertices[1].z * centerBarycentricCoords.y
	+ p_transformedVertices[2].z * centerBarycentricCoords.z;

	depth = depth * 0.5f + 0.5f;
	depth = std::max(0.0f, std::min(1.0f, depth));

	float w = p_transformedVertices[0].w * centerBarycentricCoords.x
	+ p_transformedVertices[1].w * centerBarycentricCoords.y
	+ p_transformedVertices[2].w * centerBarycentricCoords.z;

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
				uint8_t gridSideLength = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));
				uint8_t currentSampleIndex = 0;
				uint32_t color = 0;
				bool isProgramProcessed = false;

				for (uint8_t gridRow = 0; gridRow < gridSideLength; gridRow++)
				{
					for (uint8_t gridCol = 0; gridCol < gridSideLength; gridCol++)
					{
						if (currentSampleIndex >= RenderContext.Samples)
							break;

						if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(x, y)))
						{
							if (RenderContext.State & AGL_STENCIL_TEST)
							{
								if (ActiveStencilBuffer)
								{
									uint8_t stencilValue = ActiveStencilBuffer->GetPixel(x, y);
									bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);

									UpdateStencilBuffer(x, y, isStencilPassed, false);
								}

							}

							continue;
						}

						if (ActiveStencilBuffer)
						{
							uint8_t stencilValue = ActiveStencilBuffer->GetPixel(x, y);
							if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
							{
								UpdateStencilBuffer(x, y, false, true);
								continue;
							}
						}

						if (RenderContext.State & AGL_STENCIL_TEST)
						{
							UpdateStencilBuffer(x, y, true, true);
						}

						if (!isProgramProcessed && RenderContext.State & AGL_COLOR_WRITE)
						{
							const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ static_cast<float>(x), static_cast<float>(y) });

							RenderContext.Program->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

							color = PackColor(RenderContext.Program->ProcessFragment());

							isProgramProcessed = true;
						}

						if (RenderContext.State & AGL_COLOR_WRITE)
						{
							MSAABuffer->SetPixelSample(x, y, currentSampleIndex, color, depth, RenderContext.State & AGL_DEPTH_TEST);
						}

						currentSampleIndex++;
					}
				}
			}
			else
			{
				if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(x, y)))
				{
					if (RenderContext.State & AGL_STENCIL_TEST)
					{
						if (ActiveStencilBuffer)
						{
							uint8_t stencilValue = ActiveStencilBuffer->GetPixel(x, y);
							bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);

							UpdateStencilBuffer(x, y, isStencilPassed, false);
						}

					}

					continue;
				}

				if (ActiveStencilBuffer)
				{
					uint8_t stencilValue = ActiveStencilBuffer->GetPixel(x, y);
					if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
					{
						UpdateStencilBuffer(x, y, false, true);
						continue;
					}
				}

				if (RenderContext.State & AGL_STENCIL_TEST)
				{
					UpdateStencilBuffer(x, y, true, true);
				}

				if (RenderContext.FrameBufferObject->ColorBuffer != nullptr && RenderContext.State & AGL_COLOR_WRITE)
				{
					const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ static_cast<float>(x), static_cast<float>(y) });

					RenderContext.Program->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

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

void RasterizePointTile(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec4& p_point)
{
	if (p_point.x < TileBoundingBox.Min.x || p_point.x >= TileBoundingBox.Max.x 
		|| p_point.y < TileBoundingBox.Min.y || p_point.y >= TileBoundingBox.Max.y)
	{
		return;
	}

	const int width = std::min(TileBoundingBox.Max.x, static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Width : RenderContext.FrameBufferObject->DepthBuffer->Width));
	const int height = std::min(TileBoundingBox.Max.y, static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Height : RenderContext.FrameBufferObject->DepthBuffer->Height));

	const glm::vec3 centerBarycentricCoords = p_triangle.GetBarycentricCoordinates({ p_point.x, p_point.y });

	float depth = p_transformedVertices[0].z * centerBarycentricCoords.x
		+ p_transformedVertices[1].z * centerBarycentricCoords.y
		+ p_transformedVertices[2].z * centerBarycentricCoords.z;

	depth = depth * 0.5f + 0.5f;
	depth = std::max(0.0f, std::min(1.0f, depth));

	float w = p_transformedVertices[0].w * centerBarycentricCoords.x
		+ p_transformedVertices[1].w * centerBarycentricCoords.y
		+ p_transformedVertices[2].w * centerBarycentricCoords.z;

	const float scaledPointSize = RenderContext.PointSize * (1.0f / std::abs(w));
	const float halfSize = scaledPointSize * 0.5f;

	const int startX = std::max(TileBoundingBox.Min.x, std::max(0, static_cast<int>(std::floor(p_point.x - halfSize))));
	const int endX = std::min(TileBoundingBox.Max.x - 1, std::min(width - 1, static_cast<int>(std::ceil(p_point.x + halfSize))));
	const int startY = std::max(TileBoundingBox.Min.y, std::max(0, static_cast<int>(std::floor(p_point.y - halfSize))));
	const int endY = std::min(TileBoundingBox.Max.y - 1, std::min(height - 1, static_cast<int>(std::ceil(p_point.y + halfSize))));

	auto& program = GetThreadLocalShader();

	for (int y = startY; y <= endY; y++)
	{
		for (int x = startX; x <= endX; x++)
		{
			if (x < WriteBoundingBox.Min.x || x >= WriteBoundingBox.Max.x ||
				y < WriteBoundingBox.Min.y || y >= WriteBoundingBox.Max.y)
			{
				continue;
			}

			if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer != nullptr)
			{
				uint8_t gridSideLength = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));
				uint8_t currentSampleIndex = 0;
				uint32_t color = 0;
				bool isProgramProcessed = false;

				for (uint8_t gridRow = 0; gridRow < gridSideLength; gridRow++)
				{
					for (uint8_t gridCol = 0; gridCol < gridSideLength; gridCol++)
					{
						if (currentSampleIndex >= RenderContext.Samples)
							break;

						if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(x, y)))
						{
							if (RenderContext.State & AGL_STENCIL_TEST)
							{
								if (ActiveStencilBuffer)
								{
									uint8_t stencilValue = ActiveStencilBuffer->GetPixel(x, y);
									bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);

									UpdateStencilBuffer(x, y, isStencilPassed, false);
								}

							}

							continue;
						}

						if (ActiveStencilBuffer)
						{
							uint8_t stencilValue = ActiveStencilBuffer->GetPixel(x, y);
							if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
							{
								UpdateStencilBuffer(x, y, false, true);
								continue;
							}
						}

						if (RenderContext.State & AGL_STENCIL_TEST)
						{
							UpdateStencilBuffer(x, y, true, true);
						}

						if (!isProgramProcessed && RenderContext.State & AGL_COLOR_WRITE)
						{
							const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ static_cast<float>(x), static_cast<float>(y) });

							program.ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

							color = PackColor(program.ProcessFragment());

							isProgramProcessed = true;
						}

						if (RenderContext.State & AGL_COLOR_WRITE)
						{
							MSAABuffer->SetPixelSample(x, y, currentSampleIndex, color, depth, RenderContext.State & AGL_DEPTH_TEST);
						}

						currentSampleIndex++;
					}
				}
				////////////////////////////////
				/*const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ static_cast<float>(x), static_cast<float>(y) });

				if (!(RenderContext.State & AGL_DEPTH_TEST) || DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(x, y)))
				{
					program.ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

					const glm::vec4 color = program.ProcessFragment();

					for (uint8_t sampleIndex = 0; sampleIndex < RenderContext.Samples; sampleIndex++)
					{
						MSAABuffer->SetPixelSample(x, y, sampleIndex, PackColor(color), depth, RenderContext.State & AGL_DEPTH_TEST);
					}
				}*/
			}
			else
			{
				if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(depth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(x, y)))
				{
					if (RenderContext.State & AGL_STENCIL_TEST)
					{
						if (ActiveStencilBuffer)
						{
							uint8_t stencilValue = ActiveStencilBuffer->GetPixel(x, y);
							bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);

							UpdateStencilBuffer(x, y, isStencilPassed, false);
						}

					}

					continue;
				}

				if (ActiveStencilBuffer)
				{
					uint8_t stencilValue = ActiveStencilBuffer->GetPixel(x, y);
					if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
					{
						UpdateStencilBuffer(x, y, false, true);
						continue;
					}
				}

				if (RenderContext.State & AGL_STENCIL_TEST)
				{
					UpdateStencilBuffer(x, y, true, true);
				}

				if (RenderContext.FrameBufferObject->ColorBuffer != nullptr && RenderContext.State & AGL_COLOR_WRITE)
				{
					const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ static_cast<float>(x), static_cast<float>(y) });

					program.ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

					const glm::vec4 color = program.ProcessFragment();

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
						MSAABuffer->SetPixelSample(x, y, sampleIndex, PackColor(color), depth, RenderContext.State & AGL_DEPTH_TEST);
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
							MSAABuffer->SetPixelSample(pixelX, pixelY, sampleCount, PackColor(color), sampleDepth, RenderContext.State & AGL_DEPTH_TEST);

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
						RenderContext.Program->ProcessInterpolation({ 1.0f - interpolationParam, interpolationParam, 0.0f }, p_transformedVertices[0].w, p_transformedVertices[1].w, 1.0f);

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

							MSAABuffer->SetPixelSample(x, y, sampleCount, PackColor(color), sampleDepth, RenderContext.State& AGL_DEPTH_TEST);

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

void RasterizeLine(const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec4& p_start, const glm::vec4& p_end)
{
	float startX = p_start.x;
	float startY = p_start.y;
	float startZ = p_start.z;
	float startW = p_start.w;
	float endX = p_end.x;
	float endY = p_end.y;
	float endZ = p_end.z;
	float endW = p_end.w;

	const int frameBufferWidth = static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Width : RenderContext.FrameBufferObject->DepthBuffer->Width);
	const int frameBufferHeight = static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Height : RenderContext.FrameBufferObject->DepthBuffer->Height);

	float deltaX = endX - startX;
	float deltaY = endY - startY;
	float safeStartW = std::abs(startW);
	float safeEndW = std::abs(endW);

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

	if (startPointX > endPointX || startPointY > endPointY) 
	{
		return;
	}

	float invLengthSquared = 1.0f / (deltaX * deltaX + deltaY * deltaY);

	for (int scanY = startPointY; scanY <= endPointY; scanY++)
	{
		for (int scanX = startPointX; scanX <= endPointX; scanX++)
		{
			if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer)
			{
				uint8_t gridSideLength = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));
				uint8_t currentSampleIndex = 0;
				uint32_t color = 0;
				bool isProgramProcessed = false;

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
							if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(sampleDepth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(scanX, scanY)))
							{
								if (RenderContext.State & AGL_STENCIL_TEST)
								{
									if (ActiveStencilBuffer)
									{
										uint8_t stencilValue = ActiveStencilBuffer->GetPixel(scanX, scanY);
										bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);

										UpdateStencilBuffer(scanX, scanY, isStencilPassed, false);
									}

								}

								continue;
							}

							if (ActiveStencilBuffer)
							{
								uint8_t stencilValue = ActiveStencilBuffer->GetPixel(scanX, scanY);
								if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
								{
									UpdateStencilBuffer(scanX, scanY, false, true);
									continue;
								}
							}

							if (RenderContext.State & AGL_STENCIL_TEST)
							{
								UpdateStencilBuffer(scanX, scanY, true, true);
							}

							if (!isProgramProcessed && RenderContext.State & AGL_COLOR_WRITE)
							{
								const glm::vec3 barycentricWeights(1.0f - clampedT, clampedT, 0.0f);

								RenderContext.Program->ProcessInterpolation(barycentricWeights, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

								color = PackColor(RenderContext.Program->ProcessFragment());

								isProgramProcessed = true;
							}
							
							if (RenderContext.State & AGL_COLOR_WRITE)
							{
								MSAABuffer->SetPixelSample(scanX, scanY, currentSampleIndex, color, sampleDepth, RenderContext.State & AGL_DEPTH_TEST);
							}
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
					if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(pixelDepth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(scanX, scanY)))
					{
						if (RenderContext.State & AGL_STENCIL_TEST)
						{
							if (ActiveStencilBuffer)
							{
								uint8_t stencilValue = ActiveStencilBuffer->GetPixel(scanX, scanY);
								bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);

								UpdateStencilBuffer(scanX, scanY, isStencilPassed, false);
							}

						}

						continue;
					}

					if (ActiveStencilBuffer)
					{
						uint8_t stencilValue = ActiveStencilBuffer->GetPixel(scanX, scanY);
						if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
						{
							UpdateStencilBuffer(scanX, scanY, false, true);
							continue;
						}
					}

					if (RenderContext.State & AGL_STENCIL_TEST)
					{
						UpdateStencilBuffer(scanX, scanY, true, true);
					}

					if (RenderContext.FrameBufferObject->ColorBuffer != nullptr && RenderContext.State & AGL_COLOR_WRITE)
					{
						const glm::vec3 barycentricWeights(1.0f - clampedT, clampedT, 0.0f);

						RenderContext.Program->ProcessInterpolation(barycentricWeights, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

						const glm::vec4 color = RenderContext.Program->ProcessFragment();

						const uint32_t existingColor = RenderContext.FrameBufferObject->ColorBuffer->GetPixel(scanX, scanY);

						RenderContext.FrameBufferObject->ColorBuffer->SetPixel(scanX, scanY, PackColor(glm::mix(UnpackColor(existingColor), color, color.w)));
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

void RasterizeLineTile(const std::array<glm::vec4, 3>& p_transformedVertices, const glm::vec4& p_start, const glm::vec4& p_end)
{
	float startX = p_start.x;
	float startY = p_start.y;
	float startZ = p_start.z;
	float startW = p_start.w;
	float endX = p_end.x;
	float endY = p_end.y;
	float endZ = p_end.z;
	float endW = p_end.w;

	float lineMinX = std::min(startX, endX);
	float lineMaxX = std::max(startX, endX);
	float lineMinY = std::min(startY, endY);
	float lineMaxY = std::max(startY, endY);

	if (lineMaxX < TileBoundingBox.Min.x || lineMinX >= TileBoundingBox.Max.x
		|| lineMaxY < TileBoundingBox.Min.y || lineMinY >= TileBoundingBox.Max.y)
	{
		return;
	}

	const int frameBufferWidth = std::min(TileBoundingBox.Max.x, static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Width : RenderContext.FrameBufferObject->DepthBuffer->Width));
	const int frameBufferHeight = std::min(TileBoundingBox.Max.y, static_cast<int>(RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Height : RenderContext.FrameBufferObject->DepthBuffer->Height));

	float deltaX = endX - startX;
	float deltaY = endY - startY;
	float safeStartW = std::abs(startW);
	float safeEndW = std::abs(endW);

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

	int startPointX = std::max(TileBoundingBox.Min.x, std::max(0, static_cast<int>(std::floor(minX))));
	int endPointX = std::min(TileBoundingBox.Max.x - 1, std::min(frameBufferWidth - 1, static_cast<int>(std::ceil(maxX))));
	int startPointY = std::max(TileBoundingBox.Min.y, std::max(0, static_cast<int>(std::floor(minY))));
	int endPointY = std::min(TileBoundingBox.Max.y - 1, std::min(frameBufferHeight - 1, static_cast<int>(std::ceil(maxY))));

	if (startPointX > endPointX || startPointY > endPointY) 
	{
		return;
	}

	float invLengthSquared = 1.0f / (deltaX * deltaX + deltaY * deltaY);

	auto& program = GetThreadLocalShader();

	for (int scanY = startPointY; scanY <= endPointY; scanY++)
	{
		for (int scanX = startPointX; scanX <= endPointX; scanX++)
		{
			if (scanX < WriteBoundingBox.Min.x || scanX >= WriteBoundingBox.Max.x ||
				scanY < WriteBoundingBox.Min.y || scanY >= WriteBoundingBox.Max.y) 
			{
				continue;
			}

			if (RenderContext.State & AGL_MULTISAMPLE && RenderContext.FrameBufferObject->ColorBuffer)
			{
				uint8_t gridSideLength = static_cast<uint8_t>(std::ceil(std::sqrt(RenderContext.Samples)));
				uint8_t currentSampleIndex = 0;
				uint32_t color = 0;
				bool isProgramProcessed = false;

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
							if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(sampleDepth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(scanX, scanY)))
							{
								if (RenderContext.State & AGL_STENCIL_TEST)
								{
									if (ActiveStencilBuffer)
									{
										uint8_t stencilValue = ActiveStencilBuffer->GetPixel(scanX, scanY);
										bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);

										UpdateStencilBuffer(scanX, scanY, isStencilPassed, false);
									}

								}

								continue;
							}

							if (ActiveStencilBuffer)
							{
								uint8_t stencilValue = ActiveStencilBuffer->GetPixel(scanX, scanY);
								if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
								{
									UpdateStencilBuffer(scanX, scanY, false, true);
									continue;
								}
							}

							if (RenderContext.State & AGL_STENCIL_TEST)
							{
								UpdateStencilBuffer(scanX, scanY, true, true);
							}

							if (!isProgramProcessed && RenderContext.State & AGL_COLOR_WRITE)
							{
								const glm::vec3 barycentricWeights(1.0f - clampedT, clampedT, 0.0f);

								program.ProcessInterpolation(barycentricWeights, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

								color = PackColor(program.ProcessFragment());

								isProgramProcessed = true;
							}

							if (RenderContext.State & AGL_COLOR_WRITE)
							{
								MSAABuffer->SetPixelSample(scanX, scanY, currentSampleIndex, color, sampleDepth, RenderContext.State & AGL_DEPTH_TEST);
							}
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
					if (RenderContext.State & AGL_DEPTH_TEST && !DepthTest(pixelDepth, RenderContext.FrameBufferObject->DepthBuffer->GetPixel(scanX, scanY)))
					{
						if (RenderContext.State & AGL_STENCIL_TEST)
						{
							if (ActiveStencilBuffer)
							{
								uint8_t stencilValue = ActiveStencilBuffer->GetPixel(scanX, scanY);
								bool isStencilPassed = StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc);

								UpdateStencilBuffer(scanX, scanY, isStencilPassed, false);
							}

						}

						continue;
					}

					if (ActiveStencilBuffer)
					{
						uint8_t stencilValue = ActiveStencilBuffer->GetPixel(scanX, scanY);
						if (RenderContext.State & AGL_STENCIL_TEST && !StencilTestFunc(RenderContext.StencilRef, stencilValue, RenderContext.StencilMask, RenderContext.StencilFunc))
						{
							UpdateStencilBuffer(scanX, scanY, false, true);
							continue;
						}
					}

					if (RenderContext.State & AGL_STENCIL_TEST)
					{
						UpdateStencilBuffer(scanX, scanY, true, true);
					}

					if (RenderContext.FrameBufferObject->ColorBuffer != nullptr && RenderContext.State & AGL_COLOR_WRITE)
					{
						const glm::vec3 barycentricWeights(1.0f - clampedT, clampedT, 0.0f);

						program.ProcessInterpolation(barycentricWeights, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);
						const glm::vec4 color = program.ProcessFragment();

						const uint32_t existingColor = RenderContext.FrameBufferObject->ColorBuffer->GetPixel(scanX, scanY);

						RenderContext.FrameBufferObject->ColorBuffer->SetPixel(scanX, scanY, PackColor(glm::mix(UnpackColor(existingColor), color, color.w)));
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

void RasterizeTriangleWireframe(const std::array<glm::vec4, 3>& p_transformedVertices)
{
	RasterizeLine(p_transformedVertices, p_transformedVertices[0], p_transformedVertices[1]);
	RasterizeLine(p_transformedVertices, p_transformedVertices[1], p_transformedVertices[2]);
	RasterizeLine(p_transformedVertices, p_transformedVertices[2], p_transformedVertices[0]);
}

void RasterizeTriangleWireframeTile(const std::array<glm::vec4, 3>& p_transformedVertices)
{
	RasterizeLineTile(p_transformedVertices, p_transformedVertices[0], p_transformedVertices[1]);
	RasterizeLineTile(p_transformedVertices, p_transformedVertices[1], p_transformedVertices[2]);
	RasterizeLineTile(p_transformedVertices, p_transformedVertices[2], p_transformedVertices[0]);
}

void RasterizeTrianglePoints(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices)
{
	RasterizePoint(p_triangle, p_transformedVertices, p_transformedVertices[0]);
	RasterizePoint(p_triangle, p_transformedVertices, p_transformedVertices[1]);
	RasterizePoint(p_triangle, p_transformedVertices, p_transformedVertices[2]);
}

void RasterizeTrianglePointsTile(const AmberGL::Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& p_transformedVertices)
{
	RasterizePointTile(p_triangle, p_transformedVertices, p_transformedVertices[0]);
	RasterizePointTile(p_triangle, p_transformedVertices, p_transformedVertices[1]);
	RasterizePointTile(p_triangle, p_transformedVertices, p_transformedVertices[2]);
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
	{
		BoundTextureUnits[CurrentActiveTextureUnit] = nullptr;
	}
	else
	{
		BoundTextureUnits[CurrentActiveTextureUnit] = TextureObjects[CurrentTexture];
	}
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
	else if ((p_internalFormat == AGL_R32F || p_internalFormat == AGL_DEPTH_COMPONENT) && p_type == AGL_FLOAT)
	{
		//TODO: Maybe use 32 bit buffer and 8 bit. splitting depth value.
		if (texture.Data32)
		{
			delete[] texture.Data32;
			texture.Data32 = nullptr;
		}

		texture.Data32 = new float[p_width * p_height];

		if (p_data) 
		{
			std::memcpy(texture.Data32, p_data, p_width * p_height * sizeof(float) );
		}
		else 
		{
			std::fill(texture.Data32, texture.Data32 + p_width * p_height, 0.0f);
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
		//std::cout << "GetTextureObject: Texture unit " << p_textureUnit << " is out of range.\n";
		return nullptr;
	}
	return BoundTextureUnits[p_textureUnit];
}

void AmberGL::GenUniformBuffers(uint32_t p_count, uint32_t* p_buffers)
{
	for (uint32_t i = 0; i < p_count; i++)
	{
		AmberGL::SoftwareRenderer::RenderObject::UniformBufferObject uniformBufferObject;
		uniformBufferObject.ID = UniformBufferID;
		uniformBufferObject.Size = 0;
		uniformBufferObject.Data.clear();
		uniformBufferObject.BindingPoint = 0;

		UniformBufferObjects[UniformBufferID] = uniformBufferObject;
		p_buffers[i] = UniformBufferID;

		UniformBufferID++;
	}
}

void AmberGL::DeleteUniformBuffers(uint32_t p_count, const uint32_t* p_buffers)
{
	for (uint32_t i = 0; i < p_count; i++)
	{
		uint32_t id = p_buffers[i];

		UniformBufferObjects.erase(id);

		if (CurrentUniformBuffer == id)
		{
			CurrentUniformBuffer = 0;
		}
	}
}

void AmberGL::BindUniformBuffer(uint32_t p_buffer)
{
	CurrentUniformBuffer = p_buffer;
}

void AmberGL::UniformBufferData(size_t p_size, const void* p_data, uint16_t p_usage)
{
	if (CurrentUniformBuffer == 0) 
		return;

	SoftwareRenderer::RenderObject::UniformBufferObject& uniformBufferObject = UniformBufferObjects[CurrentUniformBuffer];

	uniformBufferObject.Size = p_size;
	uniformBufferObject.Data.resize(p_size);

	if (p_data)
	{
		std::memcpy(uniformBufferObject.Data.data(), p_data, p_size);
	}
}

void AmberGL::UniformBufferSubData(size_t p_offset, size_t p_size, const void* p_data)
{
	if (CurrentUniformBuffer == 0) 
		return;

	SoftwareRenderer::RenderObject::UniformBufferObject& uniformBufferObject = UniformBufferObjects[CurrentUniformBuffer];

	if (p_offset + p_size > uniformBufferObject.Size) 
		return;

	//  Only copy if data changed (is ok since data are small..)
	if (p_data && memcmp(uniformBufferObject.Data.data() + p_offset, p_data, p_size) != 0)
	{
		std::memcpy(uniformBufferObject.Data.data() + p_offset, p_data, p_size);
	}
}

void AmberGL::BindUniformBufferBase(uint32_t p_bindingPoint, uint32_t p_buffer)
{
	if (p_buffer != 0 && UniformBufferObjects.find(p_buffer) == UniformBufferObjects.end())
		return;

	if (p_buffer != 0)
	{
		UniformBufferObjects[p_buffer].BindingPoint = p_bindingPoint;
	}

	if (uniformBufferObjectBindings.size() <= p_bindingPoint)
	{
		uniformBufferObjectBindings.resize(p_bindingPoint + 1, 0);
	}

	uniformBufferObjectBindings[p_bindingPoint] = p_buffer;
}

bool AmberGL::GetUBOData(uint32_t p_bindingPoint, size_t p_offset, size_t p_size, void* p_data)
{
	if (p_bindingPoint >= uniformBufferObjectBindings.size() || uniformBufferObjectBindings[p_bindingPoint] == 0)
		return false;

	uint32_t uniformBufferObjectID = uniformBufferObjectBindings[p_bindingPoint];

	auto it = UniformBufferObjects.find(uniformBufferObjectID);

	if (it == UniformBufferObjects.end())
		return false;

	const auto& ubo = it->second;

	if (p_offset + p_size > ubo.Size)
		return false;

	if (p_data)
	{
		std::memcpy(p_data, ubo.Data.data() + p_offset, p_size);
	}

	return true;
}

uint32_t AmberGL::GetBoundUBO(uint32_t bindingPoint)
{
	if (bindingPoint >= uniformBufferObjectBindings.size()) 
		return 0;

	return uniformBufferObjectBindings[bindingPoint];
}

size_t AmberGL::GetUBOSize(uint32_t uboID)
{
	auto it = UniformBufferObjects.find(uboID);

	if (it == UniformBufferObjects.end()) 
		return 0;

	return it->second.Size;
}

void AmberGL::StencilFunc(uint16_t p_func, int p_ref, uint32_t p_mask)
{
	RenderContext.StencilFunc = p_func;
	RenderContext.StencilRef = p_ref;
	RenderContext.StencilMask = p_mask;
}

void AmberGL::StencilOp(uint16_t p_sfail, uint16_t p_dpfail, uint16_t p_dppass)
{
	RenderContext.StencilFail = p_sfail;
	RenderContext.StencilPassDepthFail = p_dpfail;
	RenderContext.StencilPassDepthPass = p_dppass;
}

void AmberGL::StencilMask(uint32_t p_mask)
{
	RenderContext.StencilWriteMask = p_mask;
}

bool AmberGL::HasActiveStencilBuffer()
{
	return RenderContext.FrameBufferObject &&
		ActiveStencilBuffer != nullptr;
}

void AmberGL::ResizeDefaultFramebuffer(uint16_t p_width, uint16_t p_height)
{
	if (BackBuffer.ColorBuffer)
		BackBuffer.ColorBuffer->Resize(p_width, p_height);
	if (BackBuffer.DepthBuffer)
		BackBuffer.DepthBuffer->Resize(p_width, p_height);
	if (BackBuffer.StencilBuffer)
		BackBuffer.StencilBuffer->Resize(p_width, p_height);

	if (FrontBuffer.ColorBuffer)
		FrontBuffer.ColorBuffer->Resize(p_width, p_height);
	if (FrontBuffer.DepthBuffer)
		FrontBuffer.DepthBuffer->Resize(p_width, p_height);
	if (FrontBuffer.StencilBuffer)
		FrontBuffer.StencilBuffer->Resize(p_width, p_height);

	if (MSAABuffer)
	{
		MSAABuffer->Resize(p_width, p_height);
	}

	Viewport(0, 0, p_width, p_height);
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
		ActiveStencilBuffer = BackBuffer.StencilBuffer;
		CurrentFrameBuffer = 0;

		RenderContext.FrameBufferObject = &BackBuffer;
	}
	else
	{
		CurrentFrameBuffer = p_frameBuffer;
		AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject& frameBufferObject = FrameBufferObjects[CurrentFrameBuffer];

		ActiveDepthBuffer = frameBufferObject.DepthBuffer;
		ActiveColorBuffer = frameBufferObject.Attachment == AGL_COLOR_ATTACHMENT ? frameBufferObject.ColorBuffer : nullptr;
		ActiveStencilBuffer = frameBufferObject.StencilBuffer;

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
			{
				frameBufferObject.AttachedTexture = nullptr;
			}
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
	switch (p_mode)
	{
	case AGL_BACK:
		CurrentReadBuffer = AGL_BACK;
		break;
	case AGL_FRONT:
		CurrentReadBuffer = AGL_FRONT;
		break;
	case AGL_COLOR_ATTACHMENT:
		if (CurrentFrameBuffer != 0)
		{
			CurrentReadBuffer = AGL_COLOR_ATTACHMENT;
		}
		else
		{
			std::cout << "ReadBuffer: AGL_COLOR_ATTACHMENT only valid with bound framebuffer\n";
			CurrentReadBuffer = AGL_BACK;
		}
		break;
	case AGL_DEPTH_ATTACHMENT:
		if (CurrentFrameBuffer != 0)
		{
			CurrentReadBuffer = AGL_DEPTH_ATTACHMENT;
		}
		else
		{
			std::cout << "ReadBuffer: AGL_DEPTH_ATTACHMENT only valid with bound framebuffer\n";
			CurrentReadBuffer = AGL_BACK;
		}
		break;
	default:
		std::cout << "ReadBuffer: Unsupported mode " << p_mode << "\n";
		CurrentReadBuffer = AGL_BACK;
		break;
	}
}

void AmberGL::CopyTexImage2D(uint32_t p_target, uint32_t p_level, uint32_t p_internalFormat, int p_x, int p_y, uint32_t p_width, uint32_t p_height, uint32_t p_border)
{
	if (p_target != AGL_TEXTURE_2D)
	{
		std::cout << "CopyTexImage2D: Only AGL_TEXTURE_2D supported\n";
		return;
	}

	if (p_level != 0)
	{
		std::cout << "CopyTexImage2D: Only level 0 supported\n";
		return;
	}

	if (p_border != 0)
	{
		std::cout << "CopyTexImage2D: Border must be 0\n";
		return;
	}

	if (CurrentTexture == 0)
	{
		std::cout << "CopyTexImage2D: No texture bound\n";
		return;
	}

	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject* sourceFrameBufferObject = nullptr;

	if (CurrentReadBuffer == AGL_FRONT)
	{
		sourceFrameBufferObject = &FrontBuffer;
	}
	else if (CurrentReadBuffer == AGL_BACK || CurrentFrameBuffer == 0)
	{
		sourceFrameBufferObject = &BackBuffer;
	}
	else
	{
		auto it = FrameBufferObjects.find(CurrentFrameBuffer);
		if (it == FrameBufferObjects.end())
		{
			std::cout << "CopyTexImage2D: Current framebuffer not found\n";
			return;
		}
		sourceFrameBufferObject = &it->second;
	}

	bool copyFromDepth = false;
	bool copyFromColor = false;

	switch (p_internalFormat)
	{
	case AGL_DEPTH_COMPONENT:
		copyFromDepth = true;
		if (!sourceFrameBufferObject->DepthBuffer)
		{
			std::cout << "CopyTexImage2D: No depth buffer available for AGL_DEPTH_COMPONENT\n";
			return;
		}
		break;

	case AGL_RGBA8:
		copyFromColor = true;
		if (!sourceFrameBufferObject->ColorBuffer)
		{
			std::cout << "CopyTexImage2D: No color buffer available for AGL_RGBA8\n";
			return;
		}
		break;

	default:
		std::cout << "CopyTexImage2D: Unsupported internal format " << p_internalFormat << "\n";
		return;
	}

	uint32_t sourceWidth;
	uint32_t sourceHeight;

	if (copyFromDepth)
	{
		sourceWidth = sourceFrameBufferObject->DepthBuffer->Width;
		sourceHeight = sourceFrameBufferObject->DepthBuffer->Height;
	}
	else
	{
		sourceWidth = sourceFrameBufferObject->ColorBuffer->Width;
		sourceHeight = sourceFrameBufferObject->ColorBuffer->Height;
	}

	int maxX = std::min(p_x + static_cast<int>(p_width), static_cast<int>(sourceWidth));
	int maxY = std::min(p_y + static_cast<int>(p_height), static_cast<int>(sourceHeight));
	int clampedX = std::max(p_x, 0);
	int clampedY = std::max(p_y, 0);

	uint32_t actualWidth = std::max(0, maxX - clampedX);
	uint32_t actualHeight = std::max(0, maxY - clampedY);

	if (actualWidth == 0 || actualHeight == 0)
	{
		std::cout << "CopyTexImage2D: Copy region is outside source buffer bounds\n";
		return;
	}

	if (copyFromDepth)
	{
		uint32_t pixelCount = actualWidth * actualHeight;

		std::vector<float> depthData(pixelCount);

		for (uint32_t y = 0; y < actualHeight; ++y)
		{
			for (uint32_t x = 0; x < actualWidth; ++x)
			{
				uint32_t index = y * actualWidth + x;
				float depth = sourceFrameBufferObject->DepthBuffer->GetPixel(clampedX + x, clampedY + y);

				depth = std::clamp(depth, 0.0f, 1.0f);
				depthData[index] = depth;
			}
		}

		TexImage2D(AGL_TEXTURE_2D, 0, AGL_R32F, actualWidth, actualHeight, 0, AGL_R32F, AGL_FLOAT, depthData.data());
	}
	else if (copyFromColor)
	{
		uint32_t bytes = actualWidth * actualHeight * 4;

		std::vector<uint8_t> textureData(bytes);

		for (uint32_t y = 0; y < actualHeight; ++y)
		{
			for (uint32_t x = 0; x < actualWidth; ++x)
			{
				uint32_t index = y * actualWidth + x;

				uint32_t color = sourceFrameBufferObject->ColorBuffer->GetPixel(clampedX + x, clampedY + y);

				textureData[index * 4 + 0] = color >> 24;
				textureData[index * 4 + 1] = color >> 16;
				textureData[index * 4 + 2] = color >> 8;
				textureData[index * 4 + 3] = color;
			}
		}

		TexImage2D(AGL_TEXTURE_2D, 0, AGL_RGBA8, actualWidth, actualHeight, 0, AGL_RGBA8, AGL_UNSIGNED_BYTE, textureData.data());
	}
}

void AmberGL::SwapBuffers()
{
	if (CurrentFrameBuffer == 0)
	{
		std::swap(FrontBuffer.ColorBuffer, BackBuffer.ColorBuffer);
		std::swap(FrontBuffer.DepthBuffer, BackBuffer.DepthBuffer);
		std::swap(FrontBuffer.StencilBuffer, BackBuffer.StencilBuffer);

		ActiveColorBuffer = BackBuffer.ColorBuffer;
		ActiveDepthBuffer = BackBuffer.DepthBuffer;
		ActiveStencilBuffer = BackBuffer.StencilBuffer;
		RenderContext.FrameBufferObject = &BackBuffer;
	}
}

uint32_t* AmberGL::GetFrontBufferData()
{
	return FrontBuffer.ColorBuffer ? FrontBuffer.ColorBuffer->Data : FrontBuffer.DepthBuffer->Data;
}

uint32_t AmberGL::GetFrontBufferRowSize()
{
	return FrontBuffer.ColorBuffer ? FrontBuffer.ColorBuffer->RowSize : FrontBuffer.DepthBuffer->RowSize;
}

void AmberGL::DeleteFrameBuffer(uint32_t p_count, const uint32_t* p_buffer)
{
	for (uint32_t i = 0; i < p_count; i++)
	{
		uint32_t id = p_buffer[i];

		FrameBufferObjects.erase(id);

		if (CurrentFrameBuffer == id)
			CurrentFrameBuffer = 0;
	}
}

uint32_t* AmberGL::GetFrameBufferData()
{
	return ActiveColorBuffer ? ActiveColorBuffer->Data : ActiveDepthBuffer->Data;
}

uint32_t AmberGL::GetFrameBufferRowSize()
{
	return ActiveColorBuffer ? ActiveColorBuffer->RowSize : ActiveDepthBuffer->RowSize;
}

uint32_t AmberGL::GetActiveFrameBufferWidth()
{
	if (RenderContext.FrameBufferObject)
	{
		if (RenderContext.FrameBufferObject->DepthBuffer)
			return RenderContext.FrameBufferObject->DepthBuffer->Width;

		if (RenderContext.FrameBufferObject->ColorBuffer)
			return RenderContext.FrameBufferObject->ColorBuffer->Width;
	}

	return BackBuffer.ColorBuffer ? BackBuffer.ColorBuffer->Width : 0;
}

uint32_t AmberGL::GetActiveFrameBufferHeight()
{
	if (RenderContext.FrameBufferObject)
	{
		if (RenderContext.FrameBufferObject->DepthBuffer)
			return RenderContext.FrameBufferObject->DepthBuffer->Height;

		if (RenderContext.FrameBufferObject->ColorBuffer)
			return RenderContext.FrameBufferObject->ColorBuffer->Height;
	}

	return BackBuffer.ColorBuffer ? BackBuffer.ColorBuffer->Height : 0;
}

void AmberGL::ResizeFrameBuffer(uint32_t p_frameBuffer, uint16_t p_width, uint16_t p_height)
{
	auto it = FrameBufferObjects.find(p_frameBuffer);
	if (it == FrameBufferObjects.end())
		return;

	auto& frameBufferObject = it->second;

	if (frameBufferObject.ColorBuffer)
	{
		frameBufferObject.ColorBuffer->Resize(p_width, p_height);
	}

	if (frameBufferObject.DepthBuffer)
	{
		frameBufferObject.DepthBuffer->Resize(p_width, p_height);
	}
}

uint32_t AmberGL::GetDepthBufferRowSize()
{
	return ActiveDepthBuffer ? ActiveDepthBuffer->RowSize : 0;
}

bool AmberGL::HasActiveColorBuffer()
{
	return ActiveColorBuffer != nullptr && ActiveColorBuffer->Data != nullptr;
}

bool AmberGL::HasActiveDepthBuffer()
{
	return ActiveDepthBuffer != nullptr && ActiveDepthBuffer->Data != nullptr;
}

void AmberGL::Initialize(uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight)
{
	RenderContext.Viewport.Width = p_rasterizationBufferWidth;
	RenderContext.Viewport.Height = p_rasterizationBufferHeight;

	BackBuffer.ColorBuffer = new AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<RGBA8>(p_rasterizationBufferWidth, p_rasterizationBufferHeight);
	BackBuffer.DepthBuffer = new AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Depth>(p_rasterizationBufferWidth, p_rasterizationBufferHeight);
	BackBuffer.StencilBuffer = new AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Stencil>(p_rasterizationBufferWidth, p_rasterizationBufferHeight);

	FrontBuffer.ColorBuffer = new AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<RGBA8>(p_rasterizationBufferWidth, p_rasterizationBufferHeight);
	FrontBuffer.DepthBuffer = new AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Depth>(p_rasterizationBufferWidth, p_rasterizationBufferHeight);
	FrontBuffer.StencilBuffer = new AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Stencil>(p_rasterizationBufferWidth, p_rasterizationBufferHeight);

	ActiveColorBuffer = BackBuffer.ColorBuffer;
	ActiveDepthBuffer = BackBuffer.DepthBuffer;
	ActiveStencilBuffer = BackBuffer.StencilBuffer;
	RenderContext.FrameBufferObject = &BackBuffer;

	MSAABuffer = new AmberGL::SoftwareRenderer::Buffers::MSAABuffer(p_rasterizationBufferWidth, p_rasterizationBufferHeight);

	InitializeClippingFrustum();

	TilesX = (RenderContext.Viewport.Width + TILE_SIZE - 1) / TILE_SIZE;
	TilesY = (RenderContext.Viewport.Height + TILE_SIZE - 1) / TILE_SIZE;

	TileCount = TilesX * TilesY;

	tileTriangleLists.resize(TileCount);

	ClippedPrimitives.reserve(1024);

	TileJobs.reserve(TileCount);
}

glm::vec2 ProjectToRaster(const glm::vec4& p_clipVertex)
{
	float invW = 1.0f / p_clipVertex.w;

	glm::vec2 ndc = { p_clipVertex.x * invW, p_clipVertex.y * invW };

	glm::vec2 norm = { (ndc.x + 1.f) * 0.5f, (1.f - ndc.y) * 0.5f };

	return
	{
		RenderContext.Viewport.X + norm.x * RenderContext.Viewport.Width - 0.5f,
		RenderContext.Viewport.Y + norm.y * RenderContext.Viewport.Height - 0.5f
	};
}

uint32_t GetTileIndex(int p_x, int p_y)
{
	return p_y * TilesX + p_x;
}

std::tuple<uint32_t, uint32_t, uint32_t, uint32_t> GetTileBounds(uint32_t p_x, uint32_t p_y)
{
	uint32_t minX = p_x * TILE_SIZE;
	uint32_t minY = p_y * TILE_SIZE;

	uint32_t maxX = std::min<uint32_t>((p_x + 1) * TILE_SIZE, RenderContext.Viewport.Width);
	uint32_t maxY = std::min<uint32_t>((p_y + 1) * TILE_SIZE,RenderContext.Viewport.Height);

	return { minX, minY, maxX, maxY };
}

void RasterizeTriangleFromClipped(const AmberGL::Geometry::Polygon& clippedPoly, uint8_t idx0, uint8_t idx1, uint8_t idx2)
{
	std::array<glm::vec4, 3> clippedVertices = {
		clippedPoly.Vertices[idx0],
		clippedPoly.Vertices[idx1],
		clippedPoly.Vertices[idx2]
	};

	AmberGL::SoftwareRenderer::Programs::AProgram& program = GetThreadLocalShader();

	auto& programVaryings = program.GetVaryings();
	uint16_t offset = 0;

	for (auto& [name, varying] : programVaryings)
	{
		uint8_t typeCount = program.GetTypeCount(varying.Type);

		for (uint8_t i = 0; i < typeCount; i++)
		{
			varying.Data[0][i] = clippedPoly.Varyings[idx0][offset + i];
			varying.Data[1][i] = clippedPoly.Varyings[idx1][offset + i];
			varying.Data[2][i] = clippedPoly.Varyings[idx2][offset + i];
		}

		offset += typeCount;
	}

	TransformAndRasterizeVertices(clippedVertices);
};

glm::vec4 GetTileDebugColor(int p_tileX, int p_tileY, uint32_t p_triangleCount)
{
	if (p_triangleCount == 0)
	{
		return glm::vec4(0.0f, 0.0f, 1.0f, 0.1f);
	}

	float intensity = std::min(p_triangleCount / 10.0f, 1.0f);

	if (intensity < 0.5f)
	{
		float normalizedIntensity = intensity * 2.0f;
		return glm::vec4(normalizedIntensity, 1.0f, 0.0f, 0.1f);
	}

	float normalizedIntensity = (intensity - 0.5f) * 2.0f;

	return glm::vec4(1.0f, 1.0f - normalizedIntensity, 0.0f, 0.1f);
}

void DrawTileDebugRect(int p_minX, int p_minY, int p_maxX, int p_maxY, const glm::vec4& p_color, bool p_isFilled)
{
	if (!RenderContext.FrameBufferObject->ColorBuffer) 
		return;

	const int frameBufferWidth = RenderContext.FrameBufferObject->ColorBuffer->Width;
	const int frameBufferHeight = RenderContext.FrameBufferObject->ColorBuffer->Height;

	p_minX = std::max(0, std::min(p_minX, frameBufferWidth - 1));
	p_minY = std::max(0, std::min(p_minY, frameBufferHeight - 1));
	p_maxX = std::max(0, std::min(p_maxX, frameBufferWidth - 1));
	p_maxY = std::max(0, std::min(p_maxY, frameBufferHeight - 1));

	uint32_t packedColor = PackColor(p_color);

	if (p_isFilled)
	{
		for (int y = p_minY; y < p_maxY; ++y)
		{
			for (int x = p_minX; x < p_maxX; ++x)
			{
				uint32_t currentColor = RenderContext.FrameBufferObject->ColorBuffer->GetPixel(x, y);

				glm::vec4 current = UnpackColor(currentColor);

				glm::vec4 blended = glm::mix(current, p_color, p_color.a);

				RenderContext.FrameBufferObject->ColorBuffer->SetPixel(x, y, PackColor(blended));
			}
		}
	}
	else
	{
		for (int x = p_minX; x < p_maxX; ++x)
		{
			if (p_minY < frameBufferHeight)
			{
				uint32_t currentColor = RenderContext.FrameBufferObject->ColorBuffer->GetPixel(x, p_minY);

				glm::vec4 current = UnpackColor(currentColor);

				glm::vec4 blended = glm::mix(current, p_color, p_color.a);

				RenderContext.FrameBufferObject->ColorBuffer->SetPixel(x, p_minY, PackColor(blended));
			}
			if (p_maxY - 1 < frameBufferHeight && p_maxY - 1 != p_minY)
			{
				uint32_t currentColor = RenderContext.FrameBufferObject->ColorBuffer->GetPixel(x, p_maxY - 1);

				glm::vec4 current = UnpackColor(currentColor);

				glm::vec4 blended = glm::mix(current, p_color, p_color.a);

				RenderContext.FrameBufferObject->ColorBuffer->SetPixel(x, p_maxY - 1, PackColor(blended));
			}
		}

		for (int y = p_minY; y < p_maxY; ++y)
		{
			if (p_minX < frameBufferWidth)
			{
				uint32_t currentColor = RenderContext.FrameBufferObject->ColorBuffer->GetPixel(p_minX, y);

				glm::vec4 current = UnpackColor(currentColor);

				glm::vec4 blended = glm::mix(current, p_color, p_color.a);

				RenderContext.FrameBufferObject->ColorBuffer->SetPixel(p_minX, y, PackColor(blended));
			}
			if (p_maxX - 1 < frameBufferWidth && p_maxX - 1 != p_minX)
			{
				uint32_t currentColor = RenderContext.FrameBufferObject->ColorBuffer->GetPixel(p_maxX - 1, y);
				glm::vec4 current = UnpackColor(currentColor);
				glm::vec4 blended = glm::mix(current, p_color, p_color.a);
				RenderContext.FrameBufferObject->ColorBuffer->SetPixel(p_maxX - 1, y, PackColor(blended));
			}
		}
	}
}

bool IsVerticesIntersectsTile(const std::vector<glm::vec2>& p_screenSpaceVertices, int p_tileMinX, int p_tileMinY, int p_tileMaxX, int p_tileMaxY)
{

	if (p_screenSpaceVertices.size() < 3) 
		return false;

	float triMinX = p_screenSpaceVertices[0].x, triMaxX = p_screenSpaceVertices[0].x;
	float triMinY = p_screenSpaceVertices[0].y, triMaxY = p_screenSpaceVertices[0].y;

	for (size_t i = 1; i < p_screenSpaceVertices.size(); i++) 
	{
		triMinX = std::min(triMinX, p_screenSpaceVertices[i].x);
		triMaxX = std::max(triMaxX, p_screenSpaceVertices[i].x);
		triMinY = std::min(triMinY, p_screenSpaceVertices[i].y);
		triMaxY = std::max(triMaxY, p_screenSpaceVertices[i].y);
	}

	return !(triMaxX < p_tileMinX || triMinX >= p_tileMaxX || triMaxY < p_tileMinY || triMinY >= p_tileMaxY);
}

static std::unordered_set<size_t> debuggedTiles;

void AmberGL::DrawElements(uint16_t p_primitiveMode, uint32_t p_indexCount)
{
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<RGBA8>* originalColorBuffer = ActiveColorBuffer;
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Depth>* originalDepthBuffer = ActiveDepthBuffer;
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Stencil>* originalStencilBuffer = ActiveStencilBuffer;

	if (CurrentFrameBuffer != 0)
	{
		AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject& frameBufferObject = FrameBufferObjects[CurrentFrameBuffer];
		ActiveDepthBuffer = frameBufferObject.DepthBuffer;
		ActiveColorBuffer = frameBufferObject.Attachment == AGL_COLOR_ATTACHMENT ? frameBufferObject.ColorBuffer : nullptr;
		ActiveStencilBuffer = frameBufferObject.StencilBuffer;
	}
	else
	{
		ActiveColorBuffer = BackBuffer.ColorBuffer;
		ActiveDepthBuffer = BackBuffer.DepthBuffer;
		ActiveStencilBuffer = BackBuffer.StencilBuffer;

	}

	AmberGL::SoftwareRenderer::RenderObject::VertexArrayObject* vao = GetBoundVertexArrayObject();
	if (!vao || vao->BoundElementBuffer == 0 || vao->BoundArrayBuffer == 0)
	{
		std::cout << "Invalid VAO or buffers for drawing!\n";

		ActiveColorBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		ActiveStencilBuffer = originalStencilBuffer;

		return;
	}

	auto itIndex = BufferObjects.find(vao->BoundElementBuffer);
	auto itVertex = BufferObjects.find(vao->BoundArrayBuffer);
	if (itIndex == BufferObjects.end() || itVertex == BufferObjects.end())
	{
		std::cout << "Buffers not found!\n";

		ActiveColorBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		ActiveStencilBuffer = originalStencilBuffer;
		return;
	}

	MSAABuffer->BindFrameBuffers(ActiveColorBuffer, ActiveDepthBuffer);

	AmberGL::SoftwareRenderer::RenderObject::BufferObject& indexBufferObject = itIndex->second;
	AmberGL::SoftwareRenderer::RenderObject::BufferObject& vertexBuffer = itVertex->second;
	AmberGL::Geometry::Vertex* vertices = reinterpret_cast<AmberGL::Geometry::Vertex*>(vertexBuffer.Data.data());
	uint32_t* indices = reinterpret_cast<uint32_t*>(indexBufferObject.Data.data());

	switch (p_primitiveMode)
	{
	case AGL_TRIANGLES:
	{
#ifdef TILE_BASED

		uint32_t currentTilesX = (RenderContext.Viewport.Width + TILE_SIZE - 1) / TILE_SIZE;
		uint32_t currentTilesY = (RenderContext.Viewport.Height + TILE_SIZE - 1) / TILE_SIZE;
		uint32_t currentTileCount = currentTilesX * currentTilesY;

		if (currentTileCount > TileCount)
		{
			TilesX = currentTilesX;
			TilesY = currentTilesY;
			TileCount = currentTileCount;
			tileTriangleLists.resize(TileCount);
			TileJobs.reserve(TileCount);
		}

		ClippedPrimitives.clear();

		if (ClippedPrimitives.capacity() < p_indexCount / 3)
		{
			ClippedPrimitives.reserve(p_indexCount / 3);
		}

		for (auto& tileList : tileTriangleLists)
		{
			tileList.clear();
		}

		const glm::mat4 modelMatrix = RenderContext.Program->GetUBOModelMatrix();
		const glm::mat4 viewMatrix = RenderContext.Program->GetUBOViewMatrix();
		const glm::mat4 projectionMatrix = RenderContext.Program->GetUBOProjectionMatrix();

		const glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

		for (size_t i = 0; i + 2 < p_indexCount; i += 3)
		{
			AmberGL::Geometry::Vertex vertex0 = vertices[indices[i]];
			AmberGL::Geometry::Vertex vertex1 = vertices[indices[i + 1]];
			AmberGL::Geometry::Vertex vertex2 = vertices[indices[i + 2]];

			////HARDCORE CULLING (early culling to avoid process vertices, relevant on CPU in some context..
			//glm::vec4 clipPosition0 = modelViewProjectionMatrix * glm::vec4(vertex0.position, 1.0f);
			//glm::vec4 clipPosition1 = modelViewProjectionMatrix * glm::vec4(vertex1.position, 1.0f);
			//glm::vec4 clipPosition2 = modelViewProjectionMatrix * glm::vec4(vertex2.position, 1.0f);
			//
			//if (clipPosition0.w < 0.0f && clipPosition1.w < 0.0f && clipPosition2.w < 0.0f)
			//{
			//	continue;
			//}
			//
			//if ((clipPosition0.x < -clipPosition0.w && clipPosition1.x < -clipPosition1.w && clipPosition2.x < -clipPosition2.w) ||
			//	(clipPosition0.x > clipPosition0.w && clipPosition1.x > clipPosition1.w && clipPosition2.x > clipPosition2.w) ||
			//	(clipPosition0.y < -clipPosition0.w && clipPosition1.y < -clipPosition1.w && clipPosition2.y < -clipPosition2.w) ||
			//	(clipPosition0.y > clipPosition0.w && clipPosition1.y > clipPosition1.w && clipPosition2.y > clipPosition2.w) ||
			//	(clipPosition0.z < -clipPosition0.w && clipPosition1.z < -clipPosition1.w && clipPosition2.z < -clipPosition2.w) ||
			//	(clipPosition0.z > clipPosition0.w && clipPosition1.z > clipPosition1.w && clipPosition2.z > clipPosition2.w))
			//{
			//	continue;
			//}

			std::array<glm::vec4, 3> processedVertices
			{
				RenderContext.Program->ProcessVertex(vertex0, 0),
				RenderContext.Program->ProcessVertex(vertex1, 1),
				RenderContext.Program->ProcessVertex(vertex2, 2)
			};

			AmberGL::Geometry::Polygon currentPolygon;
			currentPolygon.Vertices = { processedVertices[0], processedVertices[1], processedVertices[2] };
			currentPolygon.TexCoords = { vertex0.texCoords, vertex1.texCoords, vertex2.texCoords };
			currentPolygon.Normals = { vertex0.normal, vertex1.normal, vertex2.normal };
			currentPolygon.VerticesCount = 3;

			auto& programVaryings = RenderContext.Program->GetVaryings();

			int totalSize = 0;

			for (const auto& [name, varying] : programVaryings)
			{
				totalSize += RenderContext.Program->GetTypeCount(varying.Type);
			}

			currentPolygon.VaryingsDataSize = totalSize;

			uint16_t offset = 0;

			for (auto& [name, varying] : programVaryings)
			{
				uint8_t typeCount = RenderContext.Program->GetTypeCount(varying.Type);

				for (uint8_t vertIdx = 0; vertIdx < 3; vertIdx++)
				{
					for (uint8_t j = 0; j < typeCount; j++)
					{
						currentPolygon.Varyings[vertIdx][offset + j] = varying.Data[vertIdx][j];
					}
				}

				offset += typeCount;
			}

			//ClipAgainstPlane(currentPolygon, ClippingFrustum[4]);

			for (const auto& plane : ClippingFrustum)
			{
				ClipAgainstPlane(currentPolygon, plane);
			}

			if (currentPolygon.VerticesCount < 3)
			{
				continue;
			}

			ClippedPrimitives.emplace_back(currentPolygon);
		}

		for (uint32_t i = 0; i < ClippedPrimitives.size(); i++)
		{
			const AmberGL::Geometry::Polygon& poly = ClippedPrimitives[i];

			float minXf = FLT_MAX, maxXf = -FLT_MAX;
			float minYf = FLT_MAX, maxYf = -FLT_MAX;

			std::vector<glm::vec2> screenSpaceVertices;
			screenSpaceVertices.reserve(poly.VerticesCount);

			for (uint8_t j = 0; j < poly.VerticesCount; j++)
			{
				glm::vec2 screenPos = ProjectToRaster(poly.Vertices[j]);
				screenSpaceVertices.push_back(screenPos);

				minXf = std::min(minXf, screenPos.x);
				maxXf = std::max(maxXf, screenPos.x);
				minYf = std::min(minYf, screenPos.y);
				maxYf = std::max(maxYf, screenPos.y);
			}

			int minX = static_cast<int>(std::floor(minXf));
			int maxX = static_cast<int>(std::ceil(maxXf));
			int minY = static_cast<int>(std::floor(minYf));
			int maxY = static_cast<int>(std::ceil(maxYf));

			if (maxX < 0 || minX >= RenderContext.Viewport.Width ||
				maxY < 0 || minY >= RenderContext.Viewport.Height)
				continue;

			minX = std::clamp(minX, 0, static_cast<int>(RenderContext.Viewport.Width) - 1);
			maxX = std::clamp(maxX, 0, static_cast<int>(RenderContext.Viewport.Width) - 1);
			minY = std::clamp(minY, 0, static_cast<int>(RenderContext.Viewport.Height) - 1);
			maxY = std::clamp(maxY, 0, static_cast<int>(RenderContext.Viewport.Height) - 1);

			const int startTileX = std::max(0, (minX - TILE_MARGIN) / TILE_SIZE);
			const int endTileX = std::min(static_cast<int>(TilesX) - 1, (maxX + TILE_MARGIN) / TILE_SIZE);
			const int startTileY = std::max(0, (minY - TILE_MARGIN) / TILE_SIZE);
			const int endTileY = std::min(static_cast<int>(TilesY) - 1, (maxY + TILE_MARGIN) / TILE_SIZE);

			for (int ty = startTileY; ty <= endTileY; ++ty)
			{
				for (int tx = startTileX; tx <= endTileX; ++tx)
				{
					int baseTileMinX = tx * TILE_SIZE;
					int baseTileMinY = ty * TILE_SIZE;
					int baseTileMaxX = std::min((tx + 1) * TILE_SIZE, static_cast<int>(RenderContext.Viewport.Width));
					int baseTileMaxY = std::min((ty + 1) * TILE_SIZE, static_cast<int>(RenderContext.Viewport.Height));

					int expandedMinX = baseTileMinX - TILE_MARGIN;
					int expandedMinY = baseTileMinY - TILE_MARGIN;
					int expandedMaxX = baseTileMaxX + TILE_MARGIN;
					int expandedMaxY = baseTileMaxY + TILE_MARGIN;

					if (IsVerticesIntersectsTile(screenSpaceVertices, expandedMinX, expandedMinY, expandedMaxX, expandedMaxY))
					{
						uint32_t tileIndex = GetTileIndex(tx, ty);
						tileTriangleLists[tileIndex].push_back(i);
					}
				}
			}
		}

		TileJobs.clear();
		TileJobs.reserve(TileCount);

		for (uint32_t y = 0; y < TilesY; y++)
		{
			for (uint32_t x = 0; x < TilesX; x++)
			{
				uint32_t tileIndex = GetTileIndex(x, y);

				if (tileTriangleLists[tileIndex].empty())
					continue;

				TileJobs.emplace_back(std::async(std::launch::async, [&, x, y, tileIndex]
				{
					Program.reset();

					auto [tileMinX, tileMinY, tileMaxX, tileMaxY] = GetTileBounds(x, y);

					TileBoundingBox.Min.x = tileMinX - 1;
					TileBoundingBox.Min.y = tileMinY - 1;
					TileBoundingBox.Max.x = tileMaxX + 1;
					TileBoundingBox.Max.y = tileMaxY + 1;

					uint32_t frameBufferWidth = RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Width : RenderContext.FrameBufferObject->DepthBuffer->Width;
					uint32_t frameBufferHeight = RenderContext.FrameBufferObject->ColorBuffer ? RenderContext.FrameBufferObject->ColorBuffer->Height : RenderContext.FrameBufferObject->DepthBuffer->Height;

					WriteBoundingBox.Min.x = std::max(0, static_cast<int>(tileMinX));
					WriteBoundingBox.Min.y = std::max(0, static_cast<int>(tileMinY));
					WriteBoundingBox.Max.x = std::min(static_cast<int>(tileMaxX), static_cast<int>(frameBufferWidth));
					WriteBoundingBox.Max.y = std::min(static_cast<int>(tileMaxY), static_cast<int>(frameBufferHeight));

					IsTileRenderPixel = false;

					for (uint32_t triIndex : tileTriangleLists[tileIndex])
					{
						const AmberGL::Geometry::Polygon& poly = ClippedPrimitives[triIndex];

						for (uint8_t i = 0; i < poly.VerticesCount - 2; i++)
						{
							RasterizeTriangleFromClipped(poly, 0, i + 1, i + 2);
						}
					}

					if (IsDrawDebugTile && IsTileRenderPixel)
					{
						glm::vec4 color = GetTileDebugColor(x, y, tileTriangleLists[tileIndex].size());

						DrawTileDebugRect(tileMinX, tileMinY, tileMaxX, tileMaxY, color, true);
						DrawTileDebugRect(tileMinX, tileMinY, tileMaxX, tileMaxY, glm::vec4(1.0f, 1.0f, 1.0f, 0.8f), false);
					}
				}));
			}
		}

		for (std::future<void>& tileJob : TileJobs)
		{
			tileJob.get();
		}
#else
		for (size_t i = 0; i + 2 < p_indexCount; i += 3)
		{
			RasterizeTriangle(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
		}
#endif
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
		break;
	}

	ActiveColorBuffer = originalColorBuffer;
	ActiveDepthBuffer = originalDepthBuffer;
	ActiveStencilBuffer = originalStencilBuffer;
}

void AmberGL::DrawArrays(uint16_t p_primitiveMode, uint32_t p_first, uint32_t p_count)
{
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<RGBA8>* originalColorBuffer = ActiveColorBuffer;
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Depth>* originalDepthBuffer = ActiveDepthBuffer;
	AmberGL::SoftwareRenderer::RenderObject::FrameBufferObjectData<Stencil>* originalStencilBuffer = ActiveStencilBuffer;

	if (CurrentFrameBuffer != 0)
	{
		AmberGL::SoftwareRenderer::RenderObject::FrameBufferObject& frameBufferObject = FrameBufferObjects[CurrentFrameBuffer];
		ActiveDepthBuffer = frameBufferObject.DepthBuffer;
		ActiveColorBuffer = frameBufferObject.Attachment == AGL_COLOR_ATTACHMENT ? frameBufferObject.ColorBuffer : nullptr;
		ActiveStencilBuffer = frameBufferObject.StencilBuffer;
	}
	else
	{
		ActiveColorBuffer = BackBuffer.ColorBuffer;
		ActiveDepthBuffer = BackBuffer.DepthBuffer;
		ActiveStencilBuffer = BackBuffer.StencilBuffer;
	}

	AmberGL::SoftwareRenderer::RenderObject::VertexArrayObject* vao = GetBoundVertexArrayObject();
	if (!vao || vao->BoundArrayBuffer == 0)
	{
		std::cout << "Invalid VAO or buffers for drawing!\n";

		ActiveColorBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		ActiveStencilBuffer = originalStencilBuffer;
		return;
	}

	auto itVertex = BufferObjects.find(vao->BoundArrayBuffer);
	if (itVertex == BufferObjects.end())
	{
		std::cout << "Vertex buffer not found!\n";

		ActiveColorBuffer = originalColorBuffer;
		ActiveDepthBuffer = originalDepthBuffer;
		ActiveStencilBuffer = originalStencilBuffer;
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

	ActiveColorBuffer = originalColorBuffer;
	ActiveDepthBuffer = originalDepthBuffer;
	ActiveStencilBuffer = originalStencilBuffer;
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
}

void AmberGL::DrawPoint(const glm::vec3& p_point0)
{
	AmberGL::Geometry::Vertex vertex0(p_point0);

	ProcessPoint(vertex0);
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
	{
		RenderContext.PolygonMode = p_mode;
	}
	else
	{
		RenderContext.PolygonMode = AGL_FILL;
	}
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
	{
		RenderContext.CullFace = p_face;
	}
	else
	{
		RenderContext.CullFace = AGL_BACK;
	}
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
	if (!p_params) 
		return;

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
	case AGL_STENCIL_TEST:
		*p_params = (RenderContext.State & AGL_STENCIL_TEST) != 0;
		break;
	case AGL_COLOR_WRITE:
		*p_params = (RenderContext.State & AGL_COLOR_WRITE) != 0;
		break;
	default:
		*p_params = false;
		break;
	}
}

void AmberGL::GetInt(uint16_t p_name, int* p_params)
{
	if (!p_params) 
		return;

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
	case AGL_DEPTH_FUNC:
		*p_params = RenderContext.DepthFunc;
		break;
	case AGL_POLYGON:
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

void AmberGL::GetFloat(uint16_t p_name, float* p_params)
{
	if (!p_params) 
		return;

	switch (p_name)
	{
	case AGL_LINE_WIDTH:
		*p_params = RenderContext.LineWidth;
		break;
	case AGL_POINT_SIZE:
		*p_params = RenderContext.PointSize;
		break;
	default:
		*p_params = -1.0f;
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

	if (p_flags & AGL_STENCIL_BUFFER_BIT)
	{
		ActiveStencilBuffer->Clear();
	}

	if (RenderContext.State & AGL_MULTISAMPLE)
		MSAABuffer->Clear();
}

void AmberGL::Viewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height)
{
	bool sizeChanged = (RenderContext.Viewport.Width != p_width || RenderContext.Viewport.Height != p_height);

	RenderContext.Viewport.X = p_x;
	RenderContext.Viewport.Y = p_y;
	RenderContext.Viewport.Width = p_width;
	RenderContext.Viewport.Height = p_height;

	if (sizeChanged)
	{
		TilesX = (p_width + TILE_SIZE - 1) / TILE_SIZE;
		TilesY = (p_height + TILE_SIZE - 1) / TILE_SIZE;
		TileCount = TilesX * TilesY;
		tileTriangleLists.resize(TileCount);
	}
}

void AmberGL::GetViewport(uint16_t* p_x, uint16_t* p_y, uint16_t* p_width, uint16_t* p_height)
{
	if (p_x)
	{
		*p_x = RenderContext.Viewport.X;
	}

	if (p_y)
	{
		*p_y = RenderContext.Viewport.Y;
	}

	if (p_width)
	{
		*p_width = RenderContext.Viewport.Width;
	}

	if (p_height)
	{
		*p_height = RenderContext.Viewport.Height;
	}
}
