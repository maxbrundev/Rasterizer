#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

#include "Buffers/TextureBuffer.h"
#include "Buffers/DepthBuffer.h"

#include "Geometry/Vertex.h"

#include "Resources/Mesh.h"

#include "Context/Window.h"
#include "Rendering/AShader.h"

namespace Rendering
{
	enum class ECullFace : uint8_t
	{
		FRONT,
		BACK,
		FRONT_AND_BACK
	};

	struct RenderState
	{
		uint8_t DepthTest  : 1;
		uint8_t DepthWrite : 1;
		ECullFace CullFace : 2;

		RenderState() : DepthTest(true), DepthWrite(true), CullFace(ECullFace::BACK)
		{
		}
	};

	class Rasterizer
	{
	public:
		Rasterizer(Context::Window& p_window, SDL_Renderer* p_sdlRenderer, uint16_t p_rasterizationBufferWidth, uint16_t p_rasterizationBufferHeight);
		~Rasterizer() = default;

		void RasterizeMesh(const Resources::Mesh& p_mesh, AShader& p_shader);
		void RasterizeTriangle(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2, AShader& p_shader);

		void DrawPoint(const Geometry::Vertex& p_vertex0) const;
		void RasterizeLine(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, AShader& p_shader, const Data::Color& p_color);
		void Clear(const Data::Color& p_color);
		void ClearDepth();
		void SendDataToGPU();

		void OnResize(uint16_t p_width, uint16_t p_height);

		Buffers::TextureBuffer& GetTextureBuffer();

		RenderState& GetRenderState();

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

		RenderState m_state;
	};
}
