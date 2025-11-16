#include "OnyxEditor/Rendering/ARenderer.h"

#include <algorithm>
#include <iostream>
#include <vector>

#include <AmberGL/Geometry/Vertex.h>

#include "OnyxEditor/Rendering/Debug/FrameDebugger.h"
#include "OnyxEditor/Rendering/Debug/FrameDebuggerMacros.h"

#include "OnyxEditor/Resources/Mesh.h"
#include "OnyxEditor/Resources/Loaders/TextureLoader.h"

OnyxEditor::Rendering::ARenderer::ARenderer(Driver& p_driver) : m_driver(p_driver)
{
	InitializeResources();

	RegisterModelMatrixSender([this](const glm::mat4& p_modelMatrix)
	{
		m_engineUBO->SetSubData(p_modelMatrix, 0);
	});

	RegisterUserMatrixSender([this](const glm::mat4& p_userMatrix)
	{
		m_engineUBO->SetSubData(p_userMatrix, sizeof(glm::mat4) + sizeof(glm::mat4) + sizeof(glm::mat4) + sizeof(glm::vec3) + sizeof(float));
	});

	m_engineUBO = std::make_unique<Buffers::UniformBuffer>(sizeof(glm::mat4) + sizeof(glm::mat4) + sizeof(glm::mat4) + sizeof(glm::vec3) + sizeof(float) + sizeof(glm::mat4), 0);
}

OnyxEditor::Rendering::ARenderer::~ARenderer()
{
	Resources::Loaders::TextureLoader::Destroy(m_emptyTexture);

	delete m_unitQuad;
	m_unitQuad = nullptr;

	if (!m_debugCaptureTextures.empty())
	{
		AmberGL::DeleteTextures(static_cast<uint32_t>(m_debugCaptureTextures.size()), m_debugCaptureTextures.data());
		m_debugCaptureTextures.clear();
	}
}

void OnyxEditor::Rendering::ARenderer::SetViewport(uint16_t p_x, uint16_t p_y, uint16_t p_width, uint16_t p_height) const
{
	m_driver.SetViewport(p_x, p_y, p_width, p_height);
}

void OnyxEditor::Rendering::ARenderer::Clear(bool p_colorBuffer, bool p_depthBuffer, bool p_stencilBuffer, const glm::vec4& p_color)
{
	if (p_colorBuffer)
	{
		m_driver.SetClearColor(p_color.x, p_color.y, p_color.z, p_color.w);
	}

	m_driver.Clear(p_colorBuffer, p_depthBuffer, p_stencilBuffer);

	FRAME_DEBUG_RECORD_CLEAR(*this, p_colorBuffer, p_depthBuffer, p_stencilBuffer);
}

void OnyxEditor::Rendering::ARenderer::BeginFrame(const FrameInfo& p_frameDescriptor)
{
	m_frameDescriptor = p_frameDescriptor;

	if (p_frameDescriptor.OutputBuffer != nullptr)
	{
		p_frameDescriptor.OutputBuffer->Bind();
	}

	SetViewport(0, 0, p_frameDescriptor.Width, p_frameDescriptor.Height);

	Clear(p_frameDescriptor.Camera->GetClearColorBuffer(), p_frameDescriptor.Camera->GetClearDepthBuffer(), true, { p_frameDescriptor.Camera->GetClearColor(), 1.0f });

	p_frameDescriptor.Camera->ComputeMatrices(p_frameDescriptor.Width, p_frameDescriptor.Height, p_frameDescriptor.CameraPosition, p_frameDescriptor.CameraRotation);
}

void OnyxEditor::Rendering::ARenderer::EndFrame()
{
	if (m_frameDescriptor.OutputBuffer != nullptr)
	{
		m_frameDescriptor.OutputBuffer->Unbind();
	}
}

void OnyxEditor::Rendering::ARenderer::Draw(RenderState p_renderState, const Entities::Drawable& p_drawable)
{
	auto material = p_drawable.Material;
	auto mesh = p_drawable.Mesh;

	if (mesh != nullptr && material != nullptr)
	{
		if (m_modelMatrixSender)
		{
			m_modelMatrixSender(p_drawable.ModelMatrix);
		}

		auto stateMask = material->GenerateStateMask();

		p_renderState.DepthWriting = stateMask.DepthWriting;
		p_renderState.Blending = stateMask.Blendable;
		p_renderState.Culling = stateMask.FrontFaceCulling || stateMask.BackFaceCulling;
		p_renderState.DepthTest = stateMask.DepthTest;
		p_renderState.ColorWriting = stateMask.ColorWriting;

		if (p_renderState.Culling)
		{
			if (stateMask.BackFaceCulling && stateMask.FrontFaceCulling)
			{
				p_renderState.CullFace = Settings::ECullFace::FRONT_AND_BACK;
			}
			else
			{
				p_renderState.CullFace = stateMask.BackFaceCulling ? Settings::ECullFace::BACK : Settings::ECullFace::FRONT;
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

		FRAME_DEBUG_RECORD_DRAW_CALL_AUTO(*this, p_drawable, "Draw");
	}
}

void OnyxEditor::Rendering::ARenderer::RenderUnitQuad(Resources::Material* p_material)
{
	if (!m_unitQuad || !p_material)
		return;

	RenderState quadRenderState = m_driver.CreateRenderState();
	quadRenderState.DepthTest = false;
	quadRenderState.DepthWriting = false;
	quadRenderState.Culling = false;

	p_material->Bind();

	m_driver.SetRenderState(quadRenderState);

	m_unitQuad->Bind();

	const uint32_t indexCount = m_unitQuad->GetIndexCount();
	if (indexCount > 0)
	{
		m_driver.DrawElements(Settings::EPrimitiveMode::TRIANGLES, indexCount);
	}
	else
	{
		m_driver.DrawArrays(Settings::EPrimitiveMode::TRIANGLES, m_unitQuad->GetVertexCount());
	}

	m_unitQuad->Unbind();
	p_material->Unbind();

	FRAME_DEBUG_RECORD_DRAW_CALL(*this, "Draw Unit Quad", 2, 4, p_material->GetName(), "Quad");
}

void OnyxEditor::Rendering::ARenderer::RegisterModelMatrixSender(std::function<void(glm::mat4)> p_modelMatrixSender)
{
	m_modelMatrixSender = std::move(p_modelMatrixSender);
}

void OnyxEditor::Rendering::ARenderer::RegisterUserMatrixSender(std::function<void(glm::mat4)> p_userMatrixSender)
{
	m_userMatrixSender = std::move(p_userMatrixSender);
}

void OnyxEditor::Rendering::ARenderer::UpdateViewProjectionMatrices(const glm::mat4& p_viewMatrix, const glm::mat4& p_projectionMatrix, const glm::vec3& p_cameraPosition)
{
	if (m_engineUBO)
	{
		size_t offset = sizeof(glm::mat4); // Skip model matrix
		m_engineUBO->SetSubData(p_viewMatrix, offset);

		offset += sizeof(glm::mat4);
		m_engineUBO->SetSubData(p_projectionMatrix, offset);

		offset += sizeof(glm::mat4);
		m_engineUBO->SetSubData(p_cameraPosition, offset);
	}
}

void OnyxEditor::Rendering::ARenderer::UpdateTime(float p_time)
{
	if (m_engineUBO)
	{
		size_t offset = 3 * sizeof(glm::mat4) + sizeof(glm::vec3);
		m_engineUBO->SetSubData(p_time, offset);
	}
}

uint32_t OnyxEditor::Rendering::ARenderer::CaptureFrameToTexture(uint32_t width, uint32_t height)
{
	uint32_t textureID = 0;

	AmberGL::GenTextures(1, &textureID);
	AmberGL::BindTexture(AGL_TEXTURE_2D, textureID);

	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MIN_FILTER, AGL_NEAREST);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_MAG_FILTER, AGL_NEAREST);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_S, AGL_CLAMP);
	AmberGL::TexParameteri(AGL_TEXTURE_2D, AGL_TEXTURE_WRAP_T, AGL_CLAMP);

	bool hasColorBuffer = AmberGL::HasActiveColorBuffer();
	bool hasDepthBuffer = AmberGL::HasActiveDepthBuffer();

	if (hasColorBuffer)
	{
		AmberGL::ReadBuffer(AGL_BACK);
		AmberGL::CopyTexImage2D(AGL_TEXTURE_2D, 0, AGL_RGBA8, 0, 0, width, height, 0);
	}
	else if (hasDepthBuffer)
	{
		uint32_t* depthData = AmberGL::GetFrameBufferData();
		uint32_t depthRowSize = AmberGL::GetDepthBufferRowSize();

		if (depthData && depthRowSize > 0)
		{
			uint32_t depthWidth = depthRowSize / sizeof(uint32_t);
			uint32_t depthHeight = 2048; // Shadow map size, hard code todo clean

			std::vector<uint8_t> visualizedDepth(width * height * 4);

			for (uint32_t y = 0; y < height; y++)
			{
				for (uint32_t x = 0; x < width; x++)
				{
					uint32_t srcX = (x * depthWidth) / width;
					uint32_t srcY = (y * depthHeight) / height;

					if (srcX < depthWidth && srcY < depthHeight)
					{
						uint32_t depthIndex = srcY * depthWidth + srcX;

						union
						{
							uint32_t bitValue;
							float floatValue;
						}
						Depth;

						Depth.bitValue = depthData[depthIndex];

						float depth = Depth.floatValue;

						if (std::isnan(depth) || std::isinf(depth) || depth < 0.0f)
						{
							depth = 1.0f;
						}

						depth = std::clamp(depth, 0.0f, 1.0f);
						depth = std::pow(depth, 0.3f); // Gamma correction

						uint8_t d = static_cast<uint8_t>(depth * 255.0f);

						uint32_t pixelIndex = (y * width + x) * 4;

						visualizedDepth[pixelIndex + 0] = d;
						visualizedDepth[pixelIndex + 1] = d;
						visualizedDepth[pixelIndex + 2] = d;
						visualizedDepth[pixelIndex + 3] = 255;
					}
					else
					{
						uint32_t pixelIndex = (y * width + x) * 4;

						visualizedDepth[pixelIndex + 0] = 32;
						visualizedDepth[pixelIndex + 1] = 32;
						visualizedDepth[pixelIndex + 2] = 32;
						visualizedDepth[pixelIndex + 3] = 255;
					}
				}
			}

			AmberGL::TexImage2D(AGL_TEXTURE_2D, 0, AGL_RGBA8, width, height, 0, AGL_RGBA8, AGL_UNSIGNED_BYTE, visualizedDepth.data());
		}
		else
		{
			std::vector<uint8_t> grayData(width * height * 4, 64);

			for (size_t i = 3; i < grayData.size(); i += 4)
			{
				grayData[i] = 255;
			}

			AmberGL::TexImage2D(AGL_TEXTURE_2D, 0, AGL_RGBA8, width, height,0, AGL_RGBA8, AGL_UNSIGNED_BYTE, grayData.data());
		}
	}
	else
	{
		std::vector<uint8_t> emptyData(width * height * 4, 32);

		for (size_t i = 3; i < emptyData.size(); i += 4)
		{
			emptyData[i] = 255;
		}

		AmberGL::TexImage2D(AGL_TEXTURE_2D, 0, AGL_RGBA8, width, height, 0, AGL_RGBA8, AGL_UNSIGNED_BYTE, emptyData.data());
	}

	AmberGL::BindTexture(AGL_TEXTURE_2D, 0);

	m_debugCaptureTextures.push_back(textureID);

	return textureID;
}

void OnyxEditor::Rendering::ARenderer::InitializeFrameDebugger(Context::SDLDriver& sdlRenderer, uint32_t width, uint32_t height)
{
	m_frameDebugger = std::make_unique<Debug::FrameDebugger>();
}

OnyxEditor::Rendering::Debug::FrameDebugger* OnyxEditor::Rendering::ARenderer::GetFrameDebugger() const
{
	return m_frameDebugger.get();

}

void OnyxEditor::Rendering::ARenderer::InitializeResources()
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
