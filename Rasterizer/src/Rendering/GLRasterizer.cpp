#include "Rendering/GLRasterizer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

#include "Buffers/DepthBuffer.h"
#include "Buffers/MSAABuffer.h"
#include "Geometry/Plane.h"
#include "Geometry/Polygon.h"
#include "Geometry/Triangle.h"

void InitializeClippingFrustum();

void RasterizeMesh(uint8_t p_primitiveMode, const Resources::Mesh& p_mesh);
void RasterizeLine(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Data::Color& p_color);
void RasterizeTriangle(uint8_t p_primitiveMode, const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2);
void TransformAndRasterizeVertices(const uint8_t  p_primitiveMode, const std::array<glm::vec4, 3>& processedVertices);
void ComputeFragments(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices);

void SetFragment(const Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, const std::array<glm::vec4, 3>& p_transformedVertices);
void SetSampleFragment(const Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, float p_sampleX, float p_sampleY, uint8_t p_sampleIndex, const std::array<glm::vec4, 3>& p_transformedVertices);

void RasterizeTriangleWireframe(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices);
void RasterizeLine(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_start, const glm::vec4& p_end);
void RasterizeLine(const glm::vec4& p_start, const glm::vec4& p_end, const Data::Color& p_color);

void RasterizeTrianglePoints(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices);
void DrawPoint(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_point);
void DrawPoint(const glm::vec2& p_point, const Data::Color& p_color);

glm::vec3 ComputeScreenSpaceCoordinate(const glm::vec4& p_vertexWorldPosition);
glm::vec2 ComputeNormalizedDeviceCoordinate(const glm::vec3& p_vertexScreenSpacePosition);
glm::vec2 ComputeRasterSpaceCoordinate(glm::vec2 p_vertexNormalizedPosition);

void ClipAgainstPlane(Geometry::Polygon& p_polygon, const Geometry::Plane& p_plane);
void ApplyMSAA();

namespace
{
	struct RenderContext
	{
		uint8_t State              = 0;
		uint8_t PolygonMode        = GLR_FILL;
		uint8_t CullFace           = GLR_BACK;
		uint8_t SampleCount        = 0;
		Rendering::AShader* Shader = nullptr;
	};

	struct VertexArrayObject
	{
		uint32_t ID                 = 0;
		uint32_t BoundArrayBuffer   = 0;
		uint32_t BoundElementBuffer = 0;
	};

	struct BufferObject
	{
		uint32_t ID     = 0;
		uint32_t Target = 0;
		size_t Size     = 0;
		std::vector<uint8_t> Data;
	};

	RenderContext RenderContext;

	Buffers::TextureBuffer* TextureBuffer = nullptr;
	Buffers::DepthBuffer* DepthBuffer     = nullptr;
	Buffers::MSAABuffer* MSAABuffer       = nullptr;

	std::array<Geometry::Plane, 6> ClippingFrustum;

	std::unordered_map<uint32_t, VertexArrayObject> VertexArrayObjects;
	std::unordered_map<uint32_t, BufferObject> BufferObjects;

	uint32_t VertexArrayId = 1;
	uint32_t CurrentVertexArrayObject = 0;

	uint32_t BufferId = 1;
	uint32_t CurrentArrayBuffer = 0;
	uint32_t CurrentElementBuffer = 0;

	VertexArrayObject* GetBoundVertexArrayObject()
	{
		if (CurrentVertexArrayObject == 0)
			return nullptr;

		auto it = VertexArrayObjects.find(CurrentVertexArrayObject);

		return (it != VertexArrayObjects.end()) ? &it->second : nullptr;
	}
}

void GLRasterizer::GenVertexArrays(uint32_t p_count, uint32_t* p_arrays)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		VertexArrayObject vertexArrayObject;
		vertexArrayObject.ID = VertexArrayId;
		vertexArrayObject.BoundArrayBuffer = 0;
		vertexArrayObject.BoundElementBuffer = 0;
		VertexArrayObjects[VertexArrayId] = vertexArrayObject;
		p_arrays[i] = VertexArrayId;
		VertexArrayId++;
	}
}

void GLRasterizer::DeleteVertexArrays(uint32_t p_count, const uint32_t* p_arrays)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		uint32_t id = p_arrays[i];

		VertexArrayObjects.erase(id);

		if (CurrentVertexArrayObject == id)
			CurrentVertexArrayObject = 0;
	}
}

void GLRasterizer::BindVertexArray(uint32_t p_array)
{
	if (p_array != 0 && VertexArrayObjects.find(p_array) == VertexArrayObjects.end()) 
	{
		std::cout << "VertexArray with ID " << p_array << " not found!\n";
		return;
	}

	CurrentVertexArrayObject = p_array;
}

void GLRasterizer::GenBuffers(uint32_t p_count, uint32_t* p_buffers)
{
	for (uint32_t i = 0; i < p_count; i++) 
	{
		BufferObject bufferObject;
		bufferObject.ID = BufferId;
		bufferObject.Target = 0;
		bufferObject.Size = 0;
		bufferObject.Data.clear();
		BufferObjects[BufferId] = bufferObject;
		p_buffers[i] = BufferId;

		BufferId++;
	}
}

void GLRasterizer::DeleteBuffers(uint32_t p_count, const uint32_t* p_buffers)
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

void GLRasterizer::BindBuffer(uint32_t p_target, uint32_t p_buffer)
{
	if (p_target == GLR_ARRAY_BUFFER) 
	{
		CurrentArrayBuffer = p_buffer;

		if (CurrentVertexArrayObject != 0)
			VertexArrayObjects[CurrentVertexArrayObject].BoundArrayBuffer = p_buffer;
	}
	else if (p_target == GLR_ELEMENT_ARRAY_BUFFER)
	{
		CurrentElementBuffer = p_buffer;

		if (CurrentVertexArrayObject != 0)
			VertexArrayObjects[CurrentVertexArrayObject].BoundElementBuffer = p_buffer;
	}
}

void GLRasterizer::BufferData(uint32_t p_target, size_t p_size, const void* p_data)
{
	uint32_t currentBuffer = (p_target == GLR_ARRAY_BUFFER) ? CurrentArrayBuffer : (p_target == GLR_ELEMENT_ARRAY_BUFFER) ? CurrentElementBuffer : 0;

	if (currentBuffer == 0) 
	{
		std::cout << "No buffer bound for target " << p_target << "\n";
		return;
	}

	BufferObject& bufferObject = BufferObjects[currentBuffer];
	bufferObject.Target = p_target;
	bufferObject.Size = p_size;
	bufferObject.Data.resize(p_size);
	std::memcpy(bufferObject.Data.data(), p_data, p_size);
}

void GLRasterizer::Initialize(uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight)
{
	TextureBuffer = new Buffers::TextureBuffer(p_rasterizationBufferWidth, p_rasterizationBufferHeight);
	MSAABuffer    = new Buffers::MSAABuffer(p_rasterizationBufferWidth, p_rasterizationBufferHeight);
	DepthBuffer   = new Buffers::DepthBuffer(p_rasterizationBufferWidth, p_rasterizationBufferHeight);

	InitializeClippingFrustum();
}

void GLRasterizer::Clear(const Data::Color& p_color)
{
	TextureBuffer->Clear(p_color);
	MSAABuffer->Clear(p_color);
}

void GLRasterizer::ClearDepth()
{
	DepthBuffer->Clear();
}

void GLRasterizer::DrawElements(uint8_t p_primitiveMode, uint32_t p_indexCount)
{
	VertexArrayObject* vertexArrayObject = GetBoundVertexArrayObject();

	if (vertexArrayObject == nullptr) 
	{
		std::cout << "No VAO bound!\n";
		return;
	}

	if (vertexArrayObject->BoundElementBuffer == 0) 
	{
		std::cout << "No index buffer bound in VAO!\n";
		return;
	}

	if (vertexArrayObject->BoundArrayBuffer == 0)
	{
		std::cout << "No vertex buffer bound in VAO!\n";
		return;
	}

	auto itIndex = BufferObjects.find(vertexArrayObject->BoundElementBuffer);
	if (itIndex == BufferObjects.end())
	{
		std::cout << "Index buffer not found!\n";
		return;
	}

	auto itVertex = BufferObjects.find(vertexArrayObject->BoundArrayBuffer);
	if (itVertex == BufferObjects.end())
	{
		std::cout << "Vertex buffer not found!\n";
		return;
	}

	BufferObject& indexBufferObject = itIndex->second;

	BufferObject& vertexBuffer = itVertex->second;

	Geometry::Vertex* vertices = reinterpret_cast<Geometry::Vertex*>(vertexBuffer.Data.data());
	uint32_t* indices = reinterpret_cast<uint32_t*>(indexBufferObject.Data.data());

	size_t availableIndices = indexBufferObject.Size / sizeof(uint32_t);
	uint32_t indexCount = std::min(p_indexCount, static_cast<uint32_t>(availableIndices));

	for (size_t i = 0; i + 2 < indexCount; i += 3) 
	{
		RasterizeTriangle(p_primitiveMode, 
			vertices[indices[i]], 
			vertices[indices[i + 1]], 
			vertices[indices[i + 2]]);
	}

	if (RenderContext.SampleCount > 1 && (RenderContext.SampleCount & 1) == 0)
	{
		ApplyMSAA();
	}
}

void GLRasterizer::DrawArrays(uint8_t p_primitiveMode, uint32_t p_first, uint32_t p_count)
{
	VertexArrayObject* vertexArrayObject = GetBoundVertexArrayObject();

	if (vertexArrayObject == nullptr) 
	{
		std::cout << "No VAO bound!\n";
		return;
	}

	if (vertexArrayObject->BoundArrayBuffer == 0) 
	{
		std::cout << "No vertex buffer bound in VAO!\n";
		return;
	}

	auto itVertex = BufferObjects.find(vertexArrayObject->BoundArrayBuffer);
	if (itVertex == BufferObjects.end())
	{
		std::cout << "Vertex buffer not found!\n";
		return;
	}

	BufferObject& vertexBuffer = itVertex->second;
	Geometry::Vertex* vertices = reinterpret_cast<Geometry::Vertex*>(vertexBuffer.Data.data());

	for (uint32_t i = p_first; i + 2 < p_first + p_count; i += 3)
	{
		RasterizeTriangle(p_primitiveMode,
			vertices[i], 
			vertices[i + 1], 
			vertices[i + 2]);
	}

	if (RenderContext.SampleCount > 1 && (RenderContext.SampleCount & 1) == 0)
	{
		ApplyMSAA();
	}
}

void GLRasterizer::DrawElements(uint8_t p_drawMode, const Resources::Mesh& p_mesh)
{
	RasterizeMesh(p_drawMode, p_mesh);
}

void GLRasterizer::DrawLine(const glm::vec3& p_point0, const glm::vec3& p_point1, const Data::Color& p_color)
{
	RasterizeLine(Geometry::Vertex(p_point0 ), Geometry::Vertex(p_point1), p_color);
}

void GLRasterizer::UseProgram(Rendering::AShader* p_shader)
{
	RenderContext.Shader = p_shader;
}

void GLRasterizer::SetSamples(uint8_t p_samples)
{
	RenderContext.SampleCount = p_samples;
	MSAABuffer->SetSamplesAmount(p_samples);
}

void GLRasterizer::PolygonMode(uint8_t p_mode)
{
	if (p_mode <= GLR_POINT)
		RenderContext.PolygonMode = p_mode;
	else
		RenderContext.PolygonMode = GLR_FILL;
}

void GLRasterizer::Enable(uint8_t p_state)
{
	RenderContext.State |= p_state;
}

void GLRasterizer::Disable(uint8_t p_state)
{
	RenderContext.State &= ~p_state;
}

bool GLRasterizer::IsEnabled(uint8_t p_capability)
{
	return (RenderContext.State & p_capability) != 0;
}

void GLRasterizer::CullFace(uint8_t p_face)
{
	if (p_face <= GLR_FRONT_AND_BACK)
		RenderContext.CullFace = p_face;
	else
		RenderContext.CullFace = GLR_BACK;
}

void GLRasterizer::DepthMask(bool p_flag)
{
	if (p_flag) 
	{
		Enable(GLR_DEPTH_WRITE);
	}
	else 
	{
		Disable(GLR_DEPTH_WRITE);
	}
}

void GLRasterizer::GetBool(uint8_t p_name, bool* p_params)
{
	if (!p_params) return;

	switch (p_name)
	{
	case GLR_DEPTH_WRITE:
		*p_params = (RenderContext.State & GLR_DEPTH_WRITE) != 0;
		break;
	case GLR_DEPTH_TEST:
		*p_params = (RenderContext.State & GLR_DEPTH_TEST) != 0;
		break;
	case GLR_CULL_FACE:
		*p_params = (RenderContext.State & GLR_CULL_FACE) != 0;
		break;
	default:
		*p_params = false;
		break;
	}
}

void GLRasterizer::GetInt(uint8_t p_name, int* p_params)
{
	if (!p_params) return;

	switch (p_name)
	{
	case GLR_CULL_FACE:
		*p_params = RenderContext.CullFace;
		break;
	case GLR_FILL:
	case GLR_LINE:
	case GLR_POINT:
		*p_params = RenderContext.PolygonMode;
		break;
	default:
		*p_params = -1;
		break;
	}
}

void GLRasterizer::Terminate()
{
	delete TextureBuffer;
	TextureBuffer = nullptr;

	delete MSAABuffer;
	MSAABuffer = nullptr;

	delete DepthBuffer;
	DepthBuffer = nullptr;
}

Buffers::TextureBuffer* GLRasterizer::GetFrameBuffer()
{
	return TextureBuffer;
}

uint32_t* GLRasterizer::GetFrameBufferDate()
{
	return TextureBuffer->GetData();
}

Buffers::TextureBuffer& GLRasterizer::GetTextureBuffer()
{
	return *TextureBuffer;
}

void InitializeClippingFrustum()
{
	float z_near = 1.0f;
	float z_far = 100.0f;

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

	ClippingFrustum[4].Distance = z_near;
	ClippingFrustum[4].Normal.x = 0.0f;
	ClippingFrustum[4].Normal.y = 0.0f;
	ClippingFrustum[4].Normal.z = 1.0f;

	ClippingFrustum[5].Distance = z_far;
	ClippingFrustum[5].Normal.x = 0.0f;
	ClippingFrustum[5].Normal.y = 0.0f;
	ClippingFrustum[5].Normal.z = -1.0f;
}

void RasterizeMesh(uint8_t p_primitiveMode, const Resources::Mesh& p_mesh)
{
	const auto& vertices = p_mesh.GetVertices();
	const auto& indices = p_mesh.GetIndices();

	if (!indices.empty())
	{
		for (uint32_t i = 0; i < indices.size(); i += 3)
		{
			RasterizeTriangle(p_primitiveMode, vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]]);
		}
	}
	else if (vertices.size() % 3 == 0)
	{
		for (uint32_t i = 0; i < vertices.size(); i += 3)
		{
			RasterizeTriangle(p_primitiveMode, vertices[i], vertices[i + 1], vertices[i + 2]);
		}
	}

	if (RenderContext.SampleCount > 1 && (RenderContext.SampleCount & 1) == 0)
	{
		ApplyMSAA();
	}
}

void RasterizeTriangle(uint8_t p_primitiveMode, const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2)
{
	std::array<glm::vec4, 3> processedVertices{ RenderContext.Shader->ProcessVertex(p_vertex0, 0), RenderContext.Shader->ProcessVertex(p_vertex1, 1) , RenderContext.Shader->ProcessVertex(p_vertex2, 2) };

	if constexpr (CLIPPING)
	{
		Geometry::Polygon currentPoly;
		currentPoly.Vertices = { processedVertices[0], processedVertices[1], processedVertices[2] };
		currentPoly.TextCoords = { p_vertex0.textCoords, p_vertex1.textCoords, p_vertex2.textCoords };
		currentPoly.VerticesCount = 3;

		for (const auto& plane : ClippingFrustum)
		{
			ClipAgainstPlane(currentPoly, plane);
		}

		for (int i = 0; i < currentPoly.VerticesCount - 2; i++)
		{
			std::array<glm::vec4, 3> clippedVertices{ currentPoly.Vertices[0], currentPoly.Vertices[i + 1], currentPoly.Vertices[i + 2] };

			RenderContext.Shader->SetVarying<glm::vec2>("v_TextCoords", currentPoly.TextCoords[0], 0);
			RenderContext.Shader->SetVarying<glm::vec2>("v_TextCoords", currentPoly.TextCoords[i + 1], 1);
			RenderContext.Shader->SetVarying<glm::vec2>("v_TextCoords", currentPoly.TextCoords[i + 2], 2);

			TransformAndRasterizeVertices(p_primitiveMode, clippedVertices);
		}
	}
	else
	{
		TransformAndRasterizeVertices(p_primitiveMode, processedVertices);
	}
}

void TransformAndRasterizeVertices(const uint8_t p_primitiveMode, const std::array<glm::vec4, 3>& processedVertices)
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

	Geometry::Triangle triangle(vertexRasterPosition0, vertexRasterPosition1, vertexRasterPosition2);

	float area = triangle.ComputeArea();


	if ((RenderContext.CullFace == GLR_BACK && area > 0.0f) ||
		(RenderContext.CullFace == GLR_FRONT && area < 0.0f) ||
		(RenderContext.CullFace == GLR_FRONT_AND_BACK))
		return;

	if (p_primitiveMode == GLR_TRIANGLES)
	{
		switch (RenderContext.PolygonMode)
		{
		case GLR_FILL:
			ComputeFragments(triangle, transformedVertices);
			break;
		case GLR_LINE:
			RasterizeTriangleWireframe(triangle, transformedVertices);
			break;
		case GLR_POINT:
			RasterizeTrianglePoints(triangle, transformedVertices);
			break;
		}
	}
	else if (p_primitiveMode == GLR_LINES)
	{
		RasterizeTriangleWireframe(triangle, transformedVertices);
	}
	else if (p_primitiveMode == GLR_POINTS)
	{
		RasterizeTrianglePoints(triangle, transformedVertices);
	}
	
}

void ComputeFragments(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices)
{
	auto xMin = std::max(0, p_triangle.BoundingBox2D.Min.x);
	auto yMin = std::max(0, p_triangle.BoundingBox2D.Min.y);

	auto xMax = std::min(p_triangle.BoundingBox2D.Max.x, static_cast<int32_t>(TextureBuffer->GetWidth()));
	auto yMax = std::min(p_triangle.BoundingBox2D.Max.y, static_cast<int32_t>(TextureBuffer->GetHeight()));

	for (uint32_t x = xMin; x < xMax; x++)
	{
		for (uint32_t y = yMin; y < yMax; y++)
		{
			if (RenderContext.SampleCount > 1 && (RenderContext.SampleCount & 1) == 0)
			{
				for (uint8_t sampleIndex = 0; sampleIndex < RenderContext.SampleCount; sampleIndex++)
				{
					float samplePosX = static_cast<float>(x) + (static_cast<float>(sampleIndex) + 0.5f) / static_cast<float>(RenderContext.SampleCount);
					float samplePosY = static_cast<float>(y) + (static_cast<float>(sampleIndex) + 0.5f) / static_cast<float>(RenderContext.SampleCount);

					SetSampleFragment(p_triangle, x, y, samplePosX, samplePosY, sampleIndex, transformedVertices);
				}
			}
			else
			{
				SetFragment(p_triangle, x, y, transformedVertices);
			}
		}
	}
}

void SetFragment(const Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, const std::array<glm::vec4, 3>& p_transformedVertices)
{
	const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ p_x, p_y });

	if (barycentricCoords.x >= 0.0f && barycentricCoords.y >= 0.0f && barycentricCoords.x + barycentricCoords.y <= 1.0f)
	{
		const float depth = p_transformedVertices[0].z * barycentricCoords.z + p_transformedVertices[2].z * barycentricCoords.x + barycentricCoords.y * p_transformedVertices[1].z;

		if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= DepthBuffer->GetElement(p_x, p_y))
		{
			RenderContext.Shader->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

			Data::Color color = RenderContext.Shader->ProcessFragment();

			float alpha = color.a / 255.0f;

			TextureBuffer->SetPixel(p_x, p_y, Data::Color::Mix(Data::Color(TextureBuffer->GetPixel(p_x, p_y)), color, alpha));

			if (RenderContext.State & GLR_DEPTH_WRITE)
			{
				DepthBuffer->SetElement(p_x, p_y, depth);
			}
		}
	}
}


void SetSampleFragment(const Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, float p_sampleX, float p_sampleY, uint8_t p_sampleIndex, const std::array<glm::vec4, 3>& p_transformedVertices)
{
	const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ p_sampleX, p_sampleY });

	if (barycentricCoords.x >= 0.0f && barycentricCoords.y >= 0.0f && barycentricCoords.x + barycentricCoords.y <= 1.0f)
	{
		const float depth = p_transformedVertices[0].z * barycentricCoords.z + p_transformedVertices[2].z * barycentricCoords.x + barycentricCoords.y * p_transformedVertices[1].z;

		if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= DepthBuffer->GetElement(p_x, p_y))
		{
			RenderContext.Shader->ProcessInterpolation(barycentricCoords, p_transformedVertices[0].w, p_transformedVertices[1].w, p_transformedVertices[2].w);

			Data::Color color = RenderContext.Shader->ProcessFragment();

			MSAABuffer->SetPixelSample(p_x, p_y, p_sampleIndex, color, depth);
		}
	}
}

void RasterizeTriangleWireframe(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices)
{
	RasterizeLine(p_triangle, transformedVertices, transformedVertices[0], transformedVertices[1]);
	RasterizeLine(p_triangle, transformedVertices, transformedVertices[1], transformedVertices[2]);
	RasterizeLine(p_triangle, transformedVertices, transformedVertices[2], transformedVertices[0]);
}

void RasterizeLine(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_start, const glm::vec4& p_end)
{
	int x0 = static_cast<int>(p_start.x);
	int y0 = static_cast<int>(p_start.y);
	int x1 = static_cast<int>(p_end.x);
	int y1 = static_cast<int>(p_end.y);

	// Bresenham's line algorithm.
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx - dy;

	int width = static_cast<int>(TextureBuffer->GetWidth());
	int height = static_cast<int>(TextureBuffer->GetHeight());

	float totalDistance = sqrt(dx * dx + dy * dy);

	while (x0 != x1 || y0 != y1)
	{
		if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
		{
			const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ x0, y0 });

			float depth = transformedVertices[0].z * barycentricCoords.z + transformedVertices[2].z * barycentricCoords.x + barycentricCoords.y * transformedVertices[1].z;

			if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= DepthBuffer->GetElement(x0, y0))
			{
				RenderContext.Shader->ProcessInterpolation(barycentricCoords, transformedVertices[0].w, transformedVertices[1].w, transformedVertices[2].w);

				Data::Color color = RenderContext.Shader->ProcessFragment();

				float alpha = color.a / 255.0f;

				TextureBuffer->SetPixel(x0, y0, Data::Color::Mix(Data::Color(TextureBuffer->GetPixel(x0, y0)), color, alpha));

				if (RenderContext.State & GLR_DEPTH_WRITE)
				{
					DepthBuffer->SetElement(x0, y0, depth);
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

void RasterizeLine(const glm::vec4& p_start, const glm::vec4& p_end, const Data::Color& p_color)
{
	int x0 = static_cast<int>(p_start.x);
	int y0 = static_cast<int>(p_start.y);
	int x1 = static_cast<int>(p_end.x);
	int y1 = static_cast<int>(p_end.y);

	// Bresenham's line algorithm.
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int sx = x0 < x1 ? 1 : -1;
	int sy = y0 < y1 ? 1 : -1;
	int err = dx - dy;

	int width = static_cast<int>(TextureBuffer->GetWidth());
	int height = static_cast<int>(TextureBuffer->GetHeight());

	float totalDistance = sqrt(dx * dx + dy * dy);

	while (x0 != x1 || y0 != y1)
	{
		float currentDistance = sqrt((x0 - p_start.x) * (x0 - p_start.x) + (y0 - p_end.y) * (y0 - p_end.y));

		float depth = p_start.z * ((totalDistance - currentDistance) / totalDistance) + p_end.z * (currentDistance / totalDistance);

		if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
		{
			if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= DepthBuffer->GetElement(x0, y0))
			{
				TextureBuffer->SetPixel(x0, y0, p_color);

				if (RenderContext.State & GLR_DEPTH_WRITE)
				{
					DepthBuffer->SetElement(x0, y0, depth);
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

void RasterizeTrianglePoints(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices)
{
	DrawPoint(p_triangle, transformedVertices, transformedVertices[0]);
	DrawPoint(p_triangle, transformedVertices, transformedVertices[1]);
	DrawPoint(p_triangle, transformedVertices, transformedVertices[2]);
}

void DrawPoint(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_point)
{
	int width = static_cast<int>(TextureBuffer->GetWidth());
	int height = static_cast<int>(TextureBuffer->GetHeight());

	if (p_point.x >= 0 && p_point.x < width && p_point.y >= 0 && p_point.y < height)
	{
		const glm::vec3 barycentricCoords = p_triangle.GetBarycentricCoordinates({ p_point.x, p_point.y });

		const float depth = transformedVertices[0].z * barycentricCoords.z + transformedVertices[2].z * barycentricCoords.x + barycentricCoords.y * transformedVertices[1].z;

		if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= DepthBuffer->GetElement(p_point.x, p_point.y))
		{
			RenderContext.Shader->ProcessInterpolation(barycentricCoords, transformedVertices[0].w, transformedVertices[1].w, transformedVertices[2].w);

			Data::Color color = RenderContext.Shader->ProcessFragment();

			float alpha = color.a / 255.0f;

			TextureBuffer->SetPixel(p_point.x, p_point.y, Data::Color::Mix(Data::Color(TextureBuffer->GetPixel(p_point.x, p_point.y)), color, alpha));

			if (RenderContext.State & GLR_DEPTH_WRITE)
			{
				DepthBuffer->SetElement(p_point.x, p_point.y, depth);
			}
		}
	}
}

void DrawPoint(const glm::vec2& p_point, const Data::Color& p_color)
{
	int width = static_cast<int>(TextureBuffer->GetWidth());
	int height = static_cast<int>(TextureBuffer->GetHeight());

	if (p_point.x >= 0 && p_point.x < width && p_point.y >= 0 && p_point.y < height)
	{
		TextureBuffer->SetPixel(p_point.x, p_point.y, p_color);
	}
}

void RasterizeLine(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Data::Color& p_color)
{
	glm::vec4 vertexWorldPosition0 = RenderContext.Shader->ProcessVertex(p_vertex0, 0);
	glm::vec4 vertexWorldPosition1 = RenderContext.Shader->ProcessVertex(p_vertex1, 1);

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

	int width = static_cast<int>(TextureBuffer->GetWidth());
	int height = static_cast<int>(TextureBuffer->GetHeight());

	float totalDistance = sqrt(dx * dx + dy * dy);

	while (x0 != x1 || y0 != y1)
	{
		float currentDistance = sqrt((x0 - vertexRasterPosition0.x) * (x0 - vertexRasterPosition0.x) + (y0 - vertexRasterPosition0.y) * (y0 - vertexRasterPosition0.y));

		float depth = vertexScreenPosition0.z * ((totalDistance - currentDistance) / totalDistance) + vertexScreenPosition1.z * (currentDistance / totalDistance);

		if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
		{
			if (!(RenderContext.State & GLR_DEPTH_TEST) || depth <= DepthBuffer->GetElement(x0, y0))
			{
				TextureBuffer->SetPixel(x0, y0, p_color);

				if (RenderContext.State & GLR_DEPTH_WRITE)
				{
					DepthBuffer->SetElement(x0, y0, depth);
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
	p_vertexNormalizedPosition.x = std::round(p_vertexNormalizedPosition.x * TextureBuffer->GetWidth());
	p_vertexNormalizedPosition.y = std::round(p_vertexNormalizedPosition.y * TextureBuffer->GetHeight());

	return p_vertexNormalizedPosition;
}

void ClipAgainstPlane(Geometry::Polygon& p_polygon, const Geometry::Plane& p_plane)
{
	glm::vec4 insideVertices[Geometry::MAX_POLY_VERTICES_COUNT];
	glm::vec2 insideTextCoords[Geometry::MAX_POLY_VERTICES_COUNT];

	uint8_t insideVerticesCount = 0;

	glm::vec4 previousVertex = p_polygon.Vertices[p_polygon.VerticesCount > 0 ? p_polygon.VerticesCount - 1 : 0];
	glm::vec2 previousTextCoords = p_polygon.TextCoords[p_polygon.VerticesCount > 0 ? p_polygon.VerticesCount - 1 : 0];

	float previousDotValue = glm::dot(previousVertex, glm::vec4(p_plane.Normal, 0)) + p_plane.Distance * (p_plane.Distance < previousVertex.w && p_plane.Distance != 1.0f ? 1.0f : previousVertex.w);

	for (int i = 0; i < p_polygon.VerticesCount; i++)
	{
		float currentDotValue = glm::dot(p_polygon.Vertices[i], glm::vec4(p_plane.Normal, 0)) + p_plane.Distance * (p_plane.Distance < p_polygon.Vertices[i].w && p_plane.Distance != 1.0f ? 1.0f : p_polygon.Vertices[i].w);

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
				glm::lerp(previousTextCoords.x, p_polygon.TextCoords[i].x, t),
				glm::lerp(previousTextCoords.y, p_polygon.TextCoords[i].y, t)
			};

			insideVertices[insideVerticesCount] = intersectionPoint;
			insideTextCoords[insideVerticesCount] = interpolatedTextCoords;
			insideVerticesCount++;
		}

		if (currentDotValue > 0.0f)
		{
			insideVertices[insideVerticesCount] = p_polygon.Vertices[i];
			insideTextCoords[insideVerticesCount] = p_polygon.TextCoords[i];
			insideVerticesCount++;
		}

		previousDotValue = currentDotValue;
		previousVertex = p_polygon.Vertices[i];
		previousTextCoords = p_polygon.TextCoords[i];
	}

	for (int i = 0; i < insideVerticesCount; i++)
	{
		p_polygon.Vertices[i] = insideVertices[i];
		p_polygon.TextCoords[i] = insideTextCoords[i];
	}

	p_polygon.VerticesCount = insideVerticesCount;
}

void ApplyMSAA()
{
	const uint32_t width = TextureBuffer->GetWidth();
	const uint32_t height = TextureBuffer->GetHeight();

	float depth = 0.0f;

	for (uint32_t x = 0; x < width; x++)
	{
		for (uint32_t y = 0; y < height; y++)
		{
			glm::ivec4 color = { 0, 0, 0, 0 };

			depth = 0.0f;

			for (uint8_t sampleIndex = 0; sampleIndex < RenderContext.SampleCount; ++sampleIndex)
			{
				const auto& sample = MSAABuffer->GetSample(x, y, sampleIndex);

				color.x += static_cast<uint8_t>(sample.Color >> 24);
				color.y += static_cast<uint8_t>(sample.Color >> 16);
				color.z += static_cast<uint8_t>(sample.Color >> 8);
				color.w += static_cast<uint8_t>(sample.Color);

				depth += sample.Depth;
			}

			color.x /= RenderContext.SampleCount;
			color.y /= RenderContext.SampleCount;
			color.z /= RenderContext.SampleCount;
			color.w /= RenderContext.SampleCount;

			depth /= static_cast<float>(RenderContext.SampleCount);

			Data::Color sampledColorTotal(static_cast<uint8_t>(color.x), static_cast<uint8_t>(color.y), static_cast<uint8_t>(color.z), static_cast<uint8_t>(color.w));
			const float alpha = static_cast<float>(sampledColorTotal.a) / 255.0f;

			TextureBuffer->SetPixel(x, y, Data::Color::Mix(Data::Color(TextureBuffer->GetPixel(x, y)), sampledColorTotal, alpha));

			if (RenderContext.State & GLR_DEPTH_WRITE)
			{
				DepthBuffer->SetElement(x, y, depth);
			}
		}
	}
}