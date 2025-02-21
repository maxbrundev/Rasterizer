#pragma once

#include <array>

#include <glm/glm.hpp>

#include "Buffers/TextureBuffer.h"
#include "Buffers/DepthBuffer.h"
#include "Buffers/MSAABuffer.h"

#include "Rendering/AShader.h"
#include "Rendering/Settings/EDrawMode.h"

#include "Geometry/Vertex.h"
#include "Geometry/Triangle.h"
#include "Geometry/Polygon.h"
#include "Geometry/Plane.h"

#include "Resources/Mesh.h"

#include "Context/Window.h"

namespace Rendering
{
	class Rasterizer
	{
	public:
		Rasterizer(Context::Window& p_window, uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight);
		~Rasterizer() = default;
		
		void RasterizeMesh(Settings::EDrawMode p_drawMode, const Resources::Mesh& p_mesh, AShader& p_shader);

		void RasterizeLine(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, AShader& p_shader, const Data::Color& p_color);

		void Clear(const Data::Color& p_color) const;
		void ClearDepth() const;

		void SetState(uint8_t p_state);
		void SetSamples(uint8_t p_samples);

		Buffers::TextureBuffer& GetTextureBuffer();

	private:
		void InitializeClippingFrustum();

		void RasterizeTriangle(Settings::EDrawMode p_drawMode, const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2, AShader& p_shader);
		void TransformAndRasterizeVertices(const Settings::EDrawMode p_drawMode, const std::array<glm::vec4, 3>& processedVertices, AShader& p_shader);
		void ComputeFragments(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, AShader& p_shader) const;

		void SetFragment(const Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, const std::array<glm::vec4, 3>& p_transformedVertices, AShader& p_shader) const;
		void SetSampleFragment(const Geometry::Triangle& p_triangle, uint32_t p_x, uint32_t p_y, float p_sampleX, float p_sampleY, uint8_t p_sampleIndex, const std::array<glm::vec4, 3>& p_transformedVertices, AShader& p_shader) const;
		
		void RasterizeTriangleWireframe(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, AShader& p_shader) const;
		void RasterizeLine(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_start, const glm::vec4& p_end, AShader& p_shader) const;
		void RasterizeLine(const glm::vec4& p_start, const glm::vec4& p_end, const Data::Color& p_color) const;

		void RasterizeTrianglePoints(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, AShader& p_shader) const;
		void DrawPoint(const Geometry::Triangle& p_triangle, const std::array<glm::vec4, 3>& transformedVertices, const glm::vec4& p_point, AShader& p_shader) const;
		void DrawPoint(const glm::vec2& p_point, const Data::Color& p_color) const;

		glm::vec3 ComputeScreenSpaceCoordinate(const glm::vec4& p_vertexWorldPosition);
		glm::vec2 ComputeNormalizedDeviceCoordinate(const glm::vec3& p_vertexScreenSpacePosition) const;
		glm::vec2 ComputeRasterSpaceCoordinate(glm::vec2 p_vertexNormalizedPosition) const;

		void ClipAgainstPlane(Geometry::Polygon& p_polygon, const Geometry::Plane& p_plane);

		void ApplyMSAA() const;

	private:
		Context::Window& m_window;
		
		Buffers::TextureBuffer m_textureBuffer;
		Buffers::MSAABuffer m_msaaBuffer;
		Buffers::DepthBuffer m_depthBuffer;

		uint8_t m_state = 0;

		uint8_t m_sampleCount = 0;

		std::array<Geometry::Plane, 6> m_clippingFrustum;
	};
}
