#include "AmberEditor/Rendering/ARenderer.h"

#include <vector>

#include "AmberEditor/Resources/Mesh.h"
#include "AmberEditor/Resources/Loaders/TextureLoader.h"
#include "AmberGL/Geometry/Vertex.h"

AmberEditor::Rendering::ARenderer::ARenderer(Driver& p_driver) : m_driver(p_driver)
{
	InitializeResources();
}

AmberEditor::Rendering::ARenderer::~ARenderer()
{
	Resources::Loaders::TextureLoader::Destroy(m_emptyTexture);

	delete m_unitQuad;
	m_unitQuad = nullptr;
}


void AmberEditor::Rendering::ARenderer::SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const
{
	m_driver.SetViewport(p_x, p_y, p_width, p_height);
}

void AmberEditor::Rendering::ARenderer::Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer, const glm::vec4& p_color)
{
	if (p_colorBuffer)
	{
		m_driver.SetClearColor(p_color.x, p_color.y, p_color.z, p_color.w);
	}

	m_driver.Clear(p_colorBuffer, p_depthBuffer, p_stencilBuffer);
}

void AmberEditor::Rendering::ARenderer::BeginFrame(const FrameInfo& p_frameDescriptor)
{
	m_frameDescriptor = p_frameDescriptor;

	if (p_frameDescriptor.OutputBuffer != nullptr)
	{
		p_frameDescriptor.OutputBuffer->Bind();
	}

	SetViewport(0, 0, p_frameDescriptor.Width, p_frameDescriptor.Height);

	Clear(p_frameDescriptor.Camera->GetClearColorBuffer(), p_frameDescriptor.Camera->GetClearDepthBuffer(), false, { p_frameDescriptor.Camera->GetClearColor(), 1.0f });

	p_frameDescriptor.Camera->ComputeMatrices(p_frameDescriptor.Width, p_frameDescriptor.Height, p_frameDescriptor.CameraPosition, p_frameDescriptor.CameraRotation);
}

void AmberEditor::Rendering::ARenderer::EndFrame()
{
	if (m_frameDescriptor.OutputBuffer != nullptr)
	{
		m_frameDescriptor.OutputBuffer->Unbind();
	}
}

void AmberEditor::Rendering::ARenderer::Draw(RenderState p_renderState, const Entities::Drawable& p_drawable)
{
	auto material = p_drawable.Material;
	auto mesh = p_drawable.Mesh;

	if (mesh != nullptr && material != nullptr)
	{
		auto stateMask = material->GenerateStateMask();

		p_renderState.DepthWriting = stateMask.DepthWriting;
		p_renderState.Blending = stateMask.Blendable;
		p_renderState.Culling = stateMask.FrontFaceCulling || stateMask.BackFaceCulling;
		p_renderState.DepthTest = stateMask.DepthTest;

		if (p_renderState.Culling)
		{
			if (stateMask.BackFaceCulling && stateMask.FrontFaceCulling)
			{
				p_renderState.CullFace = Settings::ECullFace::FRONT_AND_BACK;
			}
			else
			{
				p_renderState.CullFace =
					stateMask.BackFaceCulling ?
					Settings::ECullFace::BACK :
					Settings::ECullFace::FRONT;
			}
		}

		material->Bind(m_emptyTexture);

		m_driver.SetRenderState(p_renderState);

		mesh->Bind();

		const uint32_t indexCount = mesh->GetIndexCount();

		if (indexCount > 0)
		{
			m_driver.DrawElements(p_drawable.PrimitiveMode, indexCount);
		}
		else
		{
			m_driver.DrawArrays(p_drawable.PrimitiveMode, mesh->GetVertexCount());
		}

		mesh->Unbind();

		material->Unbind();
	}
}

void AmberEditor::Rendering::ARenderer::InitializeResources()
{
	m_emptyTexture = Resources::Loaders::TextureLoader::CreateColor((255 << 24) | (255 << 16) | (255 << 8) | 255, Settings::ETextureFilteringMode::NEAREST, Settings::ETextureFilteringMode::NEAREST);

	const std::vector<AmberGL::Geometry::Vertex> vertices = 
	{
		{ {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f} },
		{ { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f} },
		{ { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f} },
		{ {-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f} } 
	};

	const std::vector<uint32_t> indices = 
	{
		0, 1, 2,
		0, 2, 3
	};

	m_unitQuad = new Resources::Mesh(vertices, indices, 0);
}
