#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

#include "Buffers/TextureBuffer.h"
#include "Buffers/DepthBuffer.h"

#include "Geometry/Vertex.h"

#include "Resources/Mesh.h"

#include "Context/Window.h"

namespace Core
{
	class Rasterizer
	{
	public:
		Rasterizer(Context::Window& p_window, SDL_Renderer* p_sdlRenderer);
		~Rasterizer() = default;

		void RasterizeMesh(const Resources::Mesh& p_mesh, const glm::mat4& p_mvp, const glm::mat4& p_model);
		void RasterizeTriangle(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2, const glm::mat4& p_mvp, const glm::mat4& p_model);

		void DrawPoint(const Geometry::Vertex& p_vertex0) const;
		void RasterizeLine(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const glm::mat4& p_mvp, const Data::Color& p_color);
		void Clear(const Data::Color& p_color);
		void ClearDepth();
		void SendDataToGPU();

		Buffers::TextureBuffer& GetTextureBuffer();

	private:
		glm::vec3 ComputeScreenSpaceCoordinate(const glm::vec4& p_vertexWorldPosition);
		glm::vec2 ComputeNormalizedDeviceCoordinate(const glm::vec3& p_vertexScreenSpacePosition) const;
		glm::vec2 ComputeRasterSpaceCoordinate(glm::vec2 p_vertexNormalizedPosition) const;
		glm::vec3 GetBarycentricWeights(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2, const Geometry::Vertex& p_point);

		Data::Color InterpolateColors(const Data::Color& c0, const Data::Color& c1, const Data::Color& c2, const glm::vec3& weights);

	private:
		float ComputeEdge(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2) const;

	private:
		Context::Window& m_window;
		
		Buffers::TextureBuffer m_textureBuffer;
		Buffers::DepthBuffer m_depthBuffer;

		uint16_t m_minX = 0;
		uint16_t m_maxX = 0;
		uint16_t m_minY = 0;
		uint16_t m_maxY = 0;
	};
}
