#pragma once

#include <SDL.h>
#include <glm/glm.hpp>
#include <array>

#include "EDrawMode.h"
#include "Buffers/TextureBuffer.h"
#include "Buffers/DepthBuffer.h"
#include "Buffers/MSAABuffer.h"

#include "Geometry/Vertex.h"
#include "Geometry/Plane.h"

#include "Resources/Mesh.h"

#include "Rendering/AShader.h"

#include "Context/Window.h"

namespace Rendering
{
	struct Polygon
	{
		std::array <glm::vec4, 12> Vertices;
		std::array <glm::vec2, 12> textCoords;
		uint8_t VerticesCount;
	};

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

		void RasterizeMesh(EDrawMode p_drawMode, const Resources::Mesh& p_mesh, AShader& p_shader);
		void RasterizeTriangle(EDrawMode p_drawMode, const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, const Geometry::Vertex& p_vertex2, AShader& p_shader);
		void DrawPoint(const Geometry::Vertex& p_vertex0) const;
		void RasterizeLine(const Geometry::Vertex& p_vertex0, const Geometry::Vertex& p_vertex1, AShader& p_shader, const Data::Color& p_color);
		void RasterizeLine(const glm::vec4& p_vertex0, const glm::vec4& p_vertex1, const Data::Color& p_color);
		void Clear(const Data::Color& p_color);
		void ClearDepth() const;
		void SendDataToGPU();

		void SetSamples(uint8_t p_samples);

		void OnResize(uint16_t p_width, uint16_t p_height);

		Buffers::TextureBuffer& GetTextureBuffer();

		RenderState& GetRenderState();
		
	private:
		glm::vec3 ComputeScreenSpaceCoordinate(const glm::vec4& p_vertexWorldPosition);
		glm::vec2 ComputeNormalizedDeviceCoordinate(const glm::vec3& p_vertexScreenSpacePosition) const;
		glm::vec2 ComputeRasterSpaceCoordinate(glm::vec2 p_vertexNormalizedPosition) const;

		void ClipAgainstPlane(Polygon& p_polygon, const Geometry::Plane& p_plane);

	private:
		Context::Window& m_window;
		
		Buffers::TextureBuffer m_textureBuffer;
		Buffers::MSAABuffer m_msaaBuffer;
		Buffers::DepthBuffer m_depthBuffer;

		RenderState m_state;

		uint8_t m_samples = 0;

		std::array<Geometry::Plane, 6> m_clippingFrustum;
	};
}
